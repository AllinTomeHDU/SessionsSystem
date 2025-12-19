// Copyright: Jichao Luo


#include "SteamHelperBPLibrary.h"
#include "Steamworks/Steamv151/sdk/public/steam/steam_api.h"


bool USteamHelperBPLibrary::GetAvatarBrush(const FCSteamID& SteamID, FSlateBrush& Brush)
{
	if (!SteamFriends() || !SteamUtils()) return false;

	int AvatarHandle = SteamFriends()->GetLargeFriendAvatar(SteamID.GetSteamID());
	uint32 Width = 0, Height = 0;
	if (SteamUtils()->GetImageSize(AvatarHandle, &Width, &Height))
	{
		TArray<uint8> RGBA;
		RGBA.SetNum(Width * Height * 4);
		SteamUtils()->GetImageRGBA(AvatarHandle, RGBA.GetData(), RGBA.Num());
		UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
		if (!Texture) return false;

		void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, RGBA.GetData(), RGBA.Num());
		Texture->PlatformData->Mips[0].BulkData.Unlock();

		Texture->UpdateResource();
		Brush.SetResourceObject(Texture);
		return true;
	}
	return false;
}

bool USteamHelperBPLibrary::GetPersonalUserInfo(FSteamUserInfo& UserInfo)
{
	if (!SteamUser()) return false;

	CSteamID MySteamID = SteamUser()->GetSteamID();
	UserInfo.SteamID = FString::Printf(TEXT("%llu"), MySteamID.ConvertToUint64());
	UserInfo.AccountID = FString::Printf(TEXT("%u"), MySteamID.GetAccountID());
	UserInfo.PersonaName = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
	UserInfo.IPCountry = UTF8_TO_TCHAR(SteamUtils()->GetIPCountry());
	return GetAvatarBrush(MySteamID.ConvertToUint64(), UserInfo.ProfileBrush);
}

int32 USteamHelperBPLibrary::GetFriendCount()
{
	if (!SteamFriends()) return -1;
	return SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
}

bool USteamHelperBPLibrary::GetFriendsInfo(TArray<FSteamFriendInfo>& FriendsInfo)
{
	if (!SteamFriends()) return false;

	FriendsInfo.Empty();
	int FriendCount = GetFriendCount();
	for (int i = 0; i < FriendCount; ++i)
	{
		FSteamFriendInfo Info;
		CSteamID FriendID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
		GetFriendUserInfo(FriendID.ConvertToUint64(), Info.UserInfo);

		FriendsInfo.Add(Info);
	}
	return true;
}

bool USteamHelperBPLibrary::GetFriendUserInfo(const FCSteamID& FriendID, FSteamUserInfo& Info)
{
	if (!SteamFriends()) return false;

	Info.SteamID = FString::Printf(TEXT("%llu"), FriendID.GetSteamID());
	Info.AccountID = FString::Printf(TEXT("%u"), FriendID.GetAccountID());
	Info.PersonaName = UTF8_TO_TCHAR(SteamFriends()->GetFriendPersonaName(FriendID.GetSteamID()));
	return true;
}

ESteamFriendOnlineState USteamHelperBPLibrary::GetFriendState(const FCSteamID& FriendID)
{
	if (!SteamFriends()) return ESteamFriendOnlineState();

	return static_cast<ESteamFriendOnlineState>(SteamFriends()->GetFriendPersonaState(FriendID.GetSteamID()));
}

