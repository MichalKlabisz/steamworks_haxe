#ifndef IPHONE
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include <FRESteamWorks/CSteam.h>

CSteam* g_Steam = NULL;

/*
 * general function
 */

 class HXSteam : public CSteam {
	 void DispatchEvent(const int req_type, const int response) {
		 //TODO: dispatch event! FREDispatchStatusEventAsync(AIRContext, (const uint8_t*)code, (const uint8_t*)level);
	 }
 };

value Steam_Init(){
	if(g_Steam) return val_true;

	bool ret = SteamAPI_Init();
	if(ret) g_Steam = new HXSteam();

	return ret?val_true:val_false;
}
DEFINE_PRIM(Steam_Init, 0);

value Steam_Shutdown(){
	if(g_Steam){
		SteamAPI_Shutdown();
		delete g_Steam; g_Steam = NULL;
	} return alloc_null();
}
DEFINE_PRIM(Steam_Shutdown, 0);

value __Steam_RunCallbacks(){
	SteamAPI_RunCallbacks(); return val_true;
}
DEFINE_PRIM(__Steam_RunCallbacks, 0);

value Steam_GetAppID(){
	if (!g_Steam) return alloc_int(0);
	return alloc_int(g_Steam->GetAppID());
}
DEFINE_PRIM(Steam_GetAppID, 0);

value Steam_GetPersonaName() {
	if (!g_Steam) return alloc_string("");

	return alloc_string(g_Steam->GetPersonaName().c_str());
}
DEFINE_PRIM(Steam_GetPersonaName, 0);

/*AIR_FUNC(AIRSteam_UseCrashHandler) {
	if (argc != 4) return FREBool(false);

	uint32 appID = 0;
	if (!FREGetUint32(argv[0], &appID)) return FREBool(false);

	std::string version, date, time;
	if (!FREGetString(argv[1], version)) return FREBool(false);
	if (!FREGetString(argv[2], date)) return FREBool(false);
	if (!FREGetString(argv[3], time)) return FREBool(false);

	SteamAPI_SetBreakpadAppID(appID);
	SteamAPI_UseBreakpadCrashHandler(version.c_str(), date.c_str(), time.c_str(),
		false, NULL, NULL);
	return FREBool(true);
}

/*
 * stats / achievements
 */

/*AIR_FUNC(AIRSteam_RequestStats) {
	bool ret = false;
	if (g_Steam) ret = g_Steam->RequestStats();

	SteamAPI_RunCallbacks();
	return FREBool(ret);
}

AIR_FUNC(AIRSteam_SetAchievement) {
	if (!g_Steam || argc != 1) return FREBool(false);

	bool ret = false;
	std::string name;
	if(FREGetString(argv[0], name))
		ret = g_Steam->SetAchievement(name);

	SteamAPI_RunCallbacks();
	return FREBool(ret);
}

AIR_FUNC(AIRSteam_ClearAchievement) {
	if (!g_Steam || argc != 1) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	return FREBool(g_Steam->ClearAchievement(name));
}

AIR_FUNC(AIRSteam_IsAchievement) {
	if (!g_Steam || argc != 1) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	return FREBool(g_Steam->IsAchievement(name));
}

AIR_FUNC(AIRSteam_GetStatInt) {
	if (!g_Steam || argc != 1) return FREInt(0);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREInt(0);

	int32 value = 0;
	g_Steam->GetStat(name, &value);
	return FREInt(value);
}

AIR_FUNC(AIRSteam_GetStatFloat) {
	if (!g_Steam || argc != 1) return FREFloat(0.0);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREFloat(0.0);

	float value = 0.0f;
	g_Steam->GetStat(name, &value);
	return FREFloat(value);
}

AIR_FUNC(AIRSteam_SetStatInt) {
	if (!g_Steam || argc != 2) return FREBool(false);

	std::string name;
	if (!FREGetString(argv[0], name)) return FREBool(false);

	int32 value;
	if (!FREGetInt32(argv[1], &value)) return FREBool(false);

	return FREBool(g_Steam->SetStat(name, value));
}
AIR_FUNC(AIRSteam_SetStatFloat) {
	if (!g_Steam || argc != 2) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	double value;
	if (!FREGetDouble(argv[1], &value)) return FREBool(false);

	return FREBool(g_Steam->SetStat(name, (float)value));
}

AIR_FUNC(AIRSteam_StoreStats) {
	if (!g_Steam) return FREBool(false);

	return FREBool(g_Steam->StoreStats());
}

AIR_FUNC(AIRSteam_ResetAllStats) {
	if (!g_Steam || argc != 1) return FREBool(false);

	uint32_t achievementsToo;
	if (!FREGetBool(argv[0], &achievementsToo)) return FREBool(false);

	return FREBool(g_Steam->ResetAllStats(achievementsToo != 0));
}

/*
 * cloud storage
 */

/*AIR_FUNC(AIRSteam_GetFileCount) {
	if (!g_Steam) return FREInt(0);

	return FREInt(g_Steam->GetFileCount());
}

AIR_FUNC(AIRSteam_GetFileSize) {
	if (!g_Steam || argc != 1) return FREInt(0);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREInt(0);

	return FREInt(g_Steam->GetFileSize(name));
}

AIR_FUNC(AIRSteam_FileExists) {
	if (!g_Steam || argc != 1) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	return FREBool(g_Steam->FileExists(name));
}

AIR_FUNC(AIRSteam_FileWrite) {
	if (!g_Steam || argc != 2) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	FREByteArray byteArray;
	if (FREAcquireByteArray(argv[1], &byteArray) != FRE_OK)
		return FREBool(false);

	bool ret = g_Steam->FileWrite(name, byteArray.bytes, byteArray.length);
	FREReleaseByteArray(argv[1]);

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_FileRead) {
	if (!g_Steam || argc != 2) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	FREByteArray byteArray;
	if (FREAcquireByteArray(argv[1], &byteArray) != FRE_OK)
		return FREBool(false);

	bool ret = false;
	char* data = NULL;
	uint32 size = g_Steam->FileRead(name, &data);
	if (size > 0 && size <= byteArray.length) {
		ret = true;
		memcpy(byteArray.bytes, data, size);
		delete data;
	}
	FREReleaseByteArray(argv[1]);

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_FileDelete) {
	if (!g_Steam || argc != 1) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	return FREBool(g_Steam->FileDelete(name));
}

AIR_FUNC(AIRSteam_FileShare) {
	if (!g_Steam || argc != 1) return FREBool(false);

	std::string name;
	if(!FREGetString(argv[0], name)) return FREBool(false);

	return FREBool(g_Steam->FileShare(name));
}

AIR_FUNC(AIRSteam_FileShareResult) {
	ARG_CHECK(0, FREUint64(k_UGCHandleInvalid));

	return FREUint64(g_Steam->FileShareResult());
}

AIR_FUNC(AIRSteam_IsCloudEnabledForApp) {
	if (!g_Steam) return FREBool(false);

	return FREBool(g_Steam->IsCloudEnabledForApp());
}

AIR_FUNC(AIRSteam_SetCloudEnabledForApp) {
	if (!g_Steam || argc != 1) return FREBool(false);

	uint32_t enabled = 0;
	if (!FREGetBool(argv[0], &enabled)) return FREBool(false);

	return FREBool(g_Steam->SetCloudEnabledForApp(enabled != 0));
}

AIR_FUNC(AIRSteam_GetQuota) {
	if (!g_Steam) return FREObject();

	int32 total, avail;
	if(!g_Steam->GetQuota(&total, &avail)) return FREObject();

	FREObject array = FREArray(2);
	FRESetArrayElementAt(array, 0, FREInt(total));
	FRESetArrayElementAt(array, 1, FREInt(avail));

	return array;
}

/*
 * ugc / workshop
 */

/*AIR_FUNC(AIRSteam_UGCDownload) {
	ARG_CHECK(2, FREBool(false));
	if (!g_Steam || argc != 2) return FREBool(false);

	UGCHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	int32 priority;
	if(!FREGetInt32(argv[1], &priority)) return FREBool(false);

	return FREBool(g_Steam->UGCDownload(handle, priority));
}

AIR_FUNC(AIRSteam_UGCRead) {
	ARG_CHECK(4, FREBool(false));

	UGCHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	int32 _size;
	if(!FREGetInt32(argv[1], &_size)) return FREBool(false);
	if(_size < 0) return FREBool(false);
	uint32 size = _size;

	uint32 offset;
	if(!FREGetUint32(argv[2], &offset)) return FREBool(false);

	FREByteArray byteArray;
	if (FREAcquireByteArray(argv[3], &byteArray) != FRE_OK)
		return FREBool(false);

	bool ret = false;
	char* data = NULL;
	if (size > 0 && size <= byteArray.length) {
		int32 result = g_Steam->UGCRead(handle, size, offset, &data);
		if (result != 0) {
			ret = true;
			memcpy(byteArray.bytes, data, result);
		}

		delete data;
	}
	FREReleaseByteArray(argv[3]);

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_GetUGCDownloadProgress) {
	ARG_CHECK(1, FREObject());

	UGCHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	int32 downloaded, expected;
	if(!g_Steam->GetUGCDownloadProgress(handle, &downloaded, &expected))
		return FREObject();

	FREObject array = FREArray(2);
	FRESetArrayElementAt(array, 0, FREInt(downloaded));
	FRESetArrayElementAt(array, 1, FREInt(expected));

	return array;
}

AIR_FUNC(AIRSteam_GetUGCDownloadResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.DownloadUGCResult", 0, NULL, &result, NULL);

	ARG_CHECK(1, result);

	UGCHandle_t handle;
	if (!FREGetUint64(argv[1], &handle)) return result;

	auto details = g_Steam->GetUGCDownloadResult(handle);
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "fileHandle", FREUint64(details->m_hFile));
	SET_PROP(result, "appID", FREUint(details->m_nAppID));
	SET_PROP(result, "size", FREInt(details->m_nSizeInBytes));
	SET_PROP(result, "fileName", FREString(details->m_pchFileName));
	SET_PROP(result, "owner", FREUint64(details->m_ulSteamIDOwner));

	return result;
}

AIR_FUNC(AIRSteam_PublishWorkshopFile) {
	ARG_CHECK(8, FREBool(false));

	std::string name, preview, title, description;
	uint32 appId, visibility, fileType;
	if (!FREGetString(argv[0], name) ||
	    !FREGetString(argv[1], preview) ||
	    !FREGetUint32(argv[2], &appId) ||
	    !FREGetString(argv[3], title) ||
	    !FREGetString(argv[4], description) ||
	    !FREGetUint32(argv[5], &visibility) ||
	    !FREGetUint32(argv[7], &fileType)) return FREBool(false);

	SteamParamStringArray_t tagArray;
	if(!extractParamStringArray(argv[6], &tagArray)) return FREBool(false);

	bool ret = g_Steam->PublishWorkshopFile(name, preview, appId, title, description,
		ERemoteStoragePublishedFileVisibility(visibility), &tagArray,
		EWorkshopFileType(fileType));

	delete[] tagArray.m_ppStrings;

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_PublishWorkshopFileResult) {
	ARG_CHECK(0, FREUint64(0));

	return FREUint64(g_Steam->PublishWorkshopFileResult());
}

AIR_FUNC(AIRSteam_DeletePublishedFile) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileId_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	return FREBool(g_Steam->DeletePublishedFile(handle));
}

AIR_FUNC(AIRSteam_GetPublishedFileDetails) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileId_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	return FREBool(g_Steam->GetPublishedFileDetails(handle));
}

AIR_FUNC(AIRSteam_GetPublishedFileDetailsResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.FileDetailsResult", 0, NULL, &result, NULL);

	ARG_CHECK(1, result);

	PublishedFileId_t file;
	if (!FREGetUint64(argv[1], &file)) return result;

	auto details = g_Steam->GetPublishedFileDetailsResult(file);
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "file", FREUint64(details->m_nPublishedFileId));
	SET_PROP(result, "creatorAppID", FREInt(details->m_nCreatorAppID));
	SET_PROP(result, "consumerAppID", FREInt(details->m_nConsumerAppID));
	SET_PROP(result, "title", FREString(details->m_rgchTitle));
	SET_PROP(result, "description", FREString(details->m_rgchDescription));
	SET_PROP(result, "fileHandle", FREUint64(details->m_hFile));
	SET_PROP(result, "previewFileHandle", FREUint64(details->m_hPreviewFile));
	SET_PROP(result, "owner", FREUint64(details->m_ulSteamIDOwner));
	SET_PROP(result, "timeCreated", FREUint(details->m_rtimeCreated));
	SET_PROP(result, "timeUpdated", FREUint(details->m_rtimeUpdated));
	SET_PROP(result, "visibility", FREInt(details->m_eVisibility));
	SET_PROP(result, "banned", FREBool(details->m_bBanned));
	SET_PROP(result, "tags", FREString(details->m_rgchTags));
	SET_PROP(result, "tagsTruncated", FREBool(details->m_bTagsTruncated));
	SET_PROP(result, "fileName", FREString(details->m_pchFileName));
	SET_PROP(result, "fileSize", FREInt(details->m_nFileSize));
	SET_PROP(result, "previewFileSize", FREInt(details->m_nPreviewFileSize));
	SET_PROP(result, "url", FREString(details->m_rgchURL));
	SET_PROP(result, "fileType", FREInt(details->m_eFileType));

	return result;
}

AIR_FUNC(AIRSteam_EnumerateUserPublishedFiles) {
	ARG_CHECK(1, FREBool(false));

	uint32 startIndex;
	if(!FREGetUint32(argv[0], &startIndex)) return FREBool(false);

	return FREBool(g_Steam->EnumerateUserPublishedFiles(startIndex));
}

AIR_FUNC(AIRSteam_EnumerateUserPublishedFilesResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.WorkshopFilesResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->EnumerateUserPublishedFilesResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "resultsReturned", FREInt(details->m_nResultsReturned));
	SET_PROP(result, "totalResults", FREInt(details->m_nTotalResultCount));

	FREObject ids = FREArray(details->m_nResultsReturned);
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		FRESetArrayElementAt(ids, 0, FREUint64(details->m_rgPublishedFileId[i]));
	}
	SET_PROP(result, "publishedFileId", ids);

	return result;
}

AIR_FUNC(AIRSteam_EnumeratePublishedWorkshopFiles) {
	ARG_CHECK(6, FREBool(false));

	uint32 type, start, count, days;
	if (!FREGetUint32(argv[0], &type) ||
	    !FREGetUint32(argv[1], &start) ||
	    !FREGetUint32(argv[2], &count) ||
	    !FREGetUint32(argv[3], &days)) return FREBool(false);

	SteamParamStringArray_t tags, userTags;
	if (!extractParamStringArray(argv[4], &tags) ||
	    !extractParamStringArray(argv[5], &userTags)) return FREBool(false);

	bool ret = g_Steam->EnumeratePublishedWorkshopFiles(
		EWorkshopEnumerationType(type), start, count, days, &tags, &userTags);

	delete[] tags.m_ppStrings;
	delete[] userTags.m_ppStrings;

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_EnumeratePublishedWorkshopFilesResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.WorkshopFilesResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->EnumeratePublishedWorkshopFilesResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "resultsReturned", FREInt(details->m_nResultsReturned));
	SET_PROP(result, "totalResults", FREInt(details->m_nTotalResultCount));

	FREObject ids = FREArray(details->m_nResultsReturned);
	FREObject scores = FREArray(details->m_nResultsReturned);
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		FRESetArrayElementAt(ids, i, FREUint64(details->m_rgPublishedFileId[i]));
		FRESetArrayElementAt(scores, i, FREFloat(details->m_rgScore[i]));
	}
	SET_PROP(result, "publishedFileId", ids);
	SET_PROP(result, "score", scores);

	return result;
}

AIR_FUNC(AIRSteam_EnumerateUserSubscribedFiles) {
	ARG_CHECK(1, FREBool(false));

	uint32 startIndex;
	if(!FREGetUint32(argv[0], &startIndex)) return FREBool(false);

	return FREBool(g_Steam->EnumerateUserSubscribedFiles(startIndex));
}

AIR_FUNC(AIRSteam_EnumerateUserSubscribedFilesResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.SubscribedFilesResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->EnumerateUserSubscribedFilesResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "resultsReturned", FREInt(details->m_nResultsReturned));
	SET_PROP(result, "totalResults", FREInt(details->m_nTotalResultCount));

	FREObject ids = FREArray(details->m_nResultsReturned);
	FREObject timesSubscribed = FREArray(details->m_nResultsReturned);
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		FRESetArrayElementAt(ids, i, FREUint64(details->m_rgPublishedFileId[i]));
		FRESetArrayElementAt(timesSubscribed, i, FREUint(details->m_rgRTimeSubscribed[i]));
	}
	SET_PROP(result, "publishedFileId", ids);
	SET_PROP(result, "timeSubscribed", timesSubscribed);

	return result;
}

AIR_FUNC(AIRSteam_EnumerateUserSharedWorkshopFiles) {
	ARG_CHECK(4, FREBool(false));

	uint64 steamID;
	uint32 start;
	if (!FREGetUint64(argv[0], &steamID) ||
	    !FREGetUint32(argv[1], &start)) return FREBool(false);

	SteamParamStringArray_t required, excluded;
	if (!extractParamStringArray(argv[2], &required) ||
	    !extractParamStringArray(argv[3], &excluded)) return FREBool(false);

	bool ret = g_Steam->EnumerateUserSharedWorkshopFiles(steamID, start,
		&required, &excluded);

	delete[] required.m_ppStrings;
	delete[] excluded.m_ppStrings;

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_EnumerateUserSharedWorkshopFilesResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.WorkshopFilesResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->EnumerateUserSharedWorkshopFilesResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "resultsReturned", FREInt(details->m_nResultsReturned));
	SET_PROP(result, "totalResults", FREInt(details->m_nTotalResultCount));

	FREObject ids = FREArray(details->m_nResultsReturned);
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		FRESetArrayElementAt(ids, i, FREUint64(details->m_rgPublishedFileId[i]));
	}
	SET_PROP(result, "publishedFileId", ids);

	return result;
}

AIR_FUNC(AIRSteam_EnumeratePublishedFilesByUserAction) {
	ARG_CHECK(2, FREBool(false));

	uint32 action, startIndex;
	if (!FREGetUint32(argv[0], &action) ||
	    !FREGetUint32(argv[1], &startIndex)) return FREBool(false);

	return FREBool(g_Steam->EnumeratePublishedFilesByUserAction(
		EWorkshopFileAction(action), startIndex));
}

AIR_FUNC(AIRSteam_EnumeratePublishedFilesByUserActionResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.FilesByActionResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->EnumeratePublishedFilesByUserActionResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "action", FREInt(details->m_eAction));
	SET_PROP(result, "resultsReturned", FREInt(details->m_nResultsReturned));
	SET_PROP(result, "totalResults", FREInt(details->m_nTotalResultCount));

	FREObject ids = FREArray(details->m_nResultsReturned);
	FREObject timeUpdated = FREArray(details->m_nResultsReturned);
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		FRESetArrayElementAt(ids, i, FREUint64(details->m_rgPublishedFileId[i]));
		FRESetArrayElementAt(timeUpdated, i, FREUint(details->m_rgRTimeUpdated[i]));
	}

	SET_PROP(result, "publishedFileId", ids);
	SET_PROP(result, "timeUpdated", timeUpdated);

	return result;
}

AIR_FUNC(AIRSteam_SubscribePublishedFile) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileId_t file;
	if(!FREGetUint64(argv[0], &file)) return FREBool(false);

	return FREBool(g_Steam->SubscribePublishedFile(file));
}

AIR_FUNC(AIRSteam_UnsubscribePublishedFile) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileId_t file;
	if(!FREGetUint64(argv[0], &file)) return FREBool(false);

	return FREBool(g_Steam->UnsubscribePublishedFile(file));
}

AIR_FUNC(AIRSteam_CreatePublishedFileUpdateRequest) {
	ARG_CHECK(1, FREUint64(k_PublishedFileUpdateHandleInvalid));

	PublishedFileId_t file;
	if(!FREGetUint64(argv[0], &file))
		return FREUint64(k_PublishedFileUpdateHandleInvalid);

	return FREUint64(g_Steam->CreatePublishedFileUpdateRequest(file));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileFile) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	std::string file;
	if(!FREGetString(argv[1], file)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFileFile(handle, file));
}

AIR_FUNC(AIRSteam_UpdatePublishedFilePreviewFile) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	std::string preview;
	if(!FREGetString(argv[1], preview)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFilePreviewFile(handle, preview));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileTitle) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	std::string title;
	if(!FREGetString(argv[1], title)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFileTitle(handle, title));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileDescription) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	std::string description;
	if(!FREGetString(argv[1], description)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFileDescription(handle, description));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileSetChangeDescription) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	std::string changeDesc;
	if(!FREGetString(argv[1], changeDesc)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFileSetChangeDescription(handle, changeDesc));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileVisibility) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	uint32 visibility;
	if(!FREGetUint32(argv[1], &visibility)) return FREBool(false);

	return FREBool(g_Steam->UpdatePublishedFileVisibility(handle,
		ERemoteStoragePublishedFileVisibility(visibility)));
}

AIR_FUNC(AIRSteam_UpdatePublishedFileTags) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	SteamParamStringArray_t tags;
	if(!extractParamStringArray(argv[1], &tags)) return FREBool(false);

	bool ret = g_Steam->UpdatePublishedFileTags(handle, &tags);

	delete[] tags.m_ppStrings;

	return FREBool(ret);
}

AIR_FUNC(AIRSteam_CommitPublishedFileUpdate) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileUpdateHandle_t handle;
	if(!FREGetUint64(argv[0], &handle)) return FREBool(false);

	return FREBool(g_Steam->CommitPublishedFileUpdate(handle));
}

AIR_FUNC(AIRSteam_GetPublishedItemVoteDetails) {
	ARG_CHECK(1, FREBool(false));

	PublishedFileId_t file;
	if(!FREGetUint64(argv[0], &file)) return FREBool(false);

	return FREBool(g_Steam->GetPublishedItemVoteDetails(file));
}

AIR_FUNC(AIRSteam_GetPublishedItemVoteDetailsResult) {
	FREObject result;
	FRENewObject((const uint8_t*)"com.amanitadesign.steam.ItemVoteDetailsResult", 0, NULL, &result, NULL);

	ARG_CHECK(0, result);
	auto details = g_Steam->GetPublishedItemVoteDetailsResult();
	if(!details) return result;

	SET_PROP(result, "result", FREInt(details->m_eResult));
	SET_PROP(result, "publishedFileId", FREUint64(details->m_unPublishedFileId));
	SET_PROP(result, "votesFor", FREInt(details->m_nVotesFor));
	SET_PROP(result, "votesAgainst", FREInt(details->m_nVotesAgainst));
	SET_PROP(result, "reports", FREInt(details->m_nReports));
	SET_PROP(result, "score", FREFloat(details->m_fScore));

	return result;
}

AIR_FUNC(AIRSteam_UpdateUserPublishedItemVote) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileId_t file;
	if(!FREGetUint64(argv[0], &file)) return FREBool(false);

	uint32_t upvote;
	if (!FREGetBool(argv[0], &upvote)) return FREBool(false);

	return FREBool(g_Steam->UpdateUserPublishedItemVote(file, upvote != 0));
}

AIR_FUNC(AIRSteam_SetUserPublishedFileAction) {
	ARG_CHECK(2, FREBool(false));

	PublishedFileId_t file;
	if (!FREGetUint64(argv[0], &file)) return FREBool(false);

	uint32_t action;
	if (!FREGetUint32(argv[0], &action)) return FREBool(false);

	return FREBool(g_Steam->SetUserPublishedFileAction(file,
		EWorkshopFileAction(action)));
}

/*
 * overlay
 */

/*AIR_FUNC(AIRSteam_ActivateGameOverlay) {
	ARG_CHECK(1, FREBool(false));

	std::string dialog;
	if (!FREGetString(argv[0], dialog)) return FREBool(false);

	return FREBool(g_Steam->ActivateGameOverlay(dialog));
}

AIR_FUNC(AIRSteam_ActivateGameOverlayToUser) {
	ARG_CHECK(2, FREBool(false));

	std::string dialog;
	if (!FREGetString(argv[0], dialog)) return FREBool(false);

	uint64 steamId;
	if (!FREGetUint64(argv[1], &steamId)) return FREBool(false);

	return FREBool(g_Steam->ActivateGameOverlayToUser(dialog, CSteamID(steamId)));
}

AIR_FUNC(AIRSteam_ActivateGameOverlayToWebPage) {
	ARG_CHECK(1, FREBool(false));

	std::string url;
	if(!FREGetString(argv[0], url)) return FREBool(false);

	return FREBool(g_Steam->ActivateGameOverlayToWebPage(url));
}

AIR_FUNC(AIRSteam_ActivateGameOverlayToStore) {
	ARG_CHECK(2, FREBool(false));

	uint32_t appId, flag;
	if(!FREGetUint32(argv[0], &appId)) return FREBool(false);
	if(!FREGetUint32(argv[1], &flag)) return FREBool(false);

	return FREBool(g_Steam->ActivateGameOverlayToStore(appId,
		EOverlayToStoreFlag(flag)));
}

AIR_FUNC(AIRSteam_ActivateGameOverlayInviteDialog) {
	ARG_CHECK(1, FREBool(false));

	uint64 lobbyId;
	if (!FREGetUint64(argv[0], &lobbyId)) return FREBool(false);

	return FREBool(g_Steam->ActivateGameOverlayInviteDialog(CSteamID(lobbyId)));
}

AIR_FUNC(AIRSteam_IsOverlayEnabled) {
	ARG_CHECK(0, FREBool(false));

	return FREBool(g_Steam->IsOverlayEnabled());
}

AIR_FUNC(AIRSteam_IsSubscribedApp) {
	ARG_CHECK(1, FREBool(false));

	uint32_t appId;
	if (!FREGetUint32(argv[0], &appId)) return FREBool(false);

	return FREBool(g_Steam->IsSubscribedApp(appId));
}

AIR_FUNC(AIRSteam_IsDLCInstalled) {
	ARG_CHECK(1, FREBool(false));

	uint32_t appId;
	if (!FREGetUint32(argv[0], &appId)) return FREBool(false);

	return FREBool(g_Steam->IsDLCInstalled(appId));
}

AIR_FUNC(AIRSteam_GetDLCCount) {
	ARG_CHECK(0, FREInt(0));

	return FREInt(g_Steam->GetDLCCount());
}

AIR_FUNC(AIRSteam_InstallDLC) {
	ARG_CHECK(1, FREBool(false));

	uint32_t appId;
	if (!FREGetUint32(argv[0], &appId)) return FREBool(false);

	return FREBool(g_Steam->InstallDLC(appId));
}

AIR_FUNC(AIRSteam_UninstallDLC) {
	ARG_CHECK(1, FREBool(false));

	uint32_t appId;
	if (!FREGetUint32(argv[0], &appId)) return FREBool(false);

	return FREBool(g_Steam->UninstallDLC(appId));
}

AIR_FUNC(AIRSteam_DLCInstalledResult) {
	ARG_CHECK(0, FREUint(0));

	return FREUint(g_Steam->DLCInstalledResult());
}*/