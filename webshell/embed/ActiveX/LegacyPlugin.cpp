/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */
#include "stdafx.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_Initialize
//
//	Initialize the plugin library. Your DLL global initialization
//	should be here
//
NPError 
NPP_Initialize(void)
{
	_Module.Lock();
    return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_Shutdown
//
//	shutdown the plugin library. Revert initializition
//
void 
NPP_Shutdown(void)
{
	_Module.Unlock();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// npp_GetJavaClass
//
//	Return the Java class representing this plugin
//
jref 
NPP_GetJavaClass(void)
{
	//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
	// get the Java environment. You need this information pretty much for
	// any jri (Java Runtime Interface) call. 
//	JRIEnv* env = NPN_GetJavaEnv();
	//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
	// init any classes that define native methods, or whose methods we
	// are going to use.
	// The following functions are generated by javah running on the java
	// class(es) representing this plugin. javah generates the files
	// <java class name>.h and <java class name>.c (same for any additional
	// class you may want to use)
	// Return the main java class representing this plugin (derives from
	// Plugin class on the java side)
//	use_netscape_plugin_Plugin(env);
//	use_AviObserver(env);
//	return (jref)use_AviPlayer(env);
	// if no java is used
	return NULL;
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_New
//
//	create a new plugin instance 
//	handle any instance specific code initialization here
//
NPError NP_LOADDS
NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16 mode,
                int16 argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{
	// trap a NULL ptr 
	if (instance == NULL)
	{
		return NPERR_INVALID_INSTANCE_ERROR;
	}

	// Read the parameters
	CLSID clsid = CLSID_NULL;
	tstring szName;
	PropertyList pl;

	for (int16 i = 0; i < argc; i++)
	{
		if (stricmp(argn[i], "CLSID") == 0)
		{
			USES_CONVERSION;
			CLSIDFromString(A2OLE(argv[i]), &clsid);
		}
		else if (stricmp(argn[i], "NAME") == 0)
		{
			USES_CONVERSION;
			szName = tstring(A2T(argv[i]));
		}
		else if (strnicmp(argn[i], "PARAM_", 6) == 0)
		{
			tstring szName(argn[i] + 6);
	
			// Empty parameters are ignored
			if (szName.empty())
			{
				continue;
			}

			// TODO check for existing params with the same name

			CComVariant vsValue(argv[i]);
			CComVariant vIValue; // Value converted to int
			CComVariant vRValue; // Value converted to real
			CComVariant &vValue = vsValue;

			// See if the variant can be converted to an integer
			if (VariantChangeType(&vIValue, &vsValue, 0, VT_I4) == S_OK)
			{
				vValue = vIValue;
			}
			else if (VariantChangeType(&vRValue, &vsValue, 0, VT_R8) == S_OK)
			{
				vValue = vRValue;
			}

			// Add named parameter to list
			Property p;
			p.szName = szName.c_str();
			p.vValue = vValue;
			pl.push_back(p);
		}
	}

	// Create the control site
	CControlSiteInstance *pSite = NULL;
	CControlSiteInstance::CreateInstance(&pSite);
	if (pSite == NULL)
	{
		return NPERR_GENERIC_ERROR;
	}
	pSite->AddRef();

	// Create the object
	pSite->Create(clsid, pl, szName);

    instance->pdata = pSite;

	return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_Destroy
//
//	Deletes a plug-in instance and releases all of its resources.
//
NPError NP_LOADDS
NPP_Destroy(NPP instance, NPSavedData** save)
{
	// Destroy the site
	CControlSiteInstance *pSite = (CControlSiteInstance *) instance->pdata;
	if (pSite)
	{
		pSite->Detach();
		pSite->Release();
	}
	instance->pdata = 0;

    return NPERR_NO_ERROR;

}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_SetWindow
//
//	Associates a platform specific window handle with a plug-in instance.
//		Called multiple times while, e.g., scrolling.  Can be called for three
//		reasons:
//
//			1.  A new window has been created
//			2.  A window has been moved or resized
//			3.  A window has been destroyed
//
//	There is also the degenerate case;  that it was called spuriously, and
//  the window handle and or coords may have or have not changed, or
//  the window handle and or coords may be ZERO.  State information
//  must be maintained by the plug-in to correctly handle the degenerate
//  case.
//
NPError NP_LOADDS
NPP_SetWindow(NPP instance, NPWindow* window)
{    
	// Reject silly parameters
	if (!window)
	{
		return NPERR_GENERIC_ERROR;
	}
	if (!instance)
	{
		return  NPERR_INVALID_INSTANCE_ERROR;
	}

	CControlSiteInstance *pSite = (CControlSiteInstance *) instance->pdata;
	if (pSite == NULL)
	{
		return NPERR_GENERIC_ERROR;
	}

	HWND hwndParent = (HWND) window->window;
	if (hwndParent)
	{
		RECT rcPos;
		GetClientRect(hwndParent, &rcPos);

		if (pSite->GetParentWindow() == NULL)
		{
			pSite->Attach(hwndParent, rcPos, NULL);
		}
		else
		{
			pSite->SetPosition(rcPos);
		}
	}

	return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_NewStream
//
//	Notifies the plugin of a new data stream.
//  The data type of the stream (a MIME name) is provided.
//  The stream object indicates whether it is seekable.
//  The plugin specifies how it wants to handle the stream.
//
//  In this case, I set the streamtype to be NPAsFile.  This tells the Navigator
//  that the plugin doesn't handle streaming and can only deal with the object as
//  a complete disk file.  It will still call the write functions but it will also
//  pass the filename of the cached file in a later NPE_StreamAsFile call when it
//  is done transfering the file.
//
//  If a plugin handles the data in a streaming manner, it should set streamtype to
//  NPNormal  (e.g. *streamtype = NPNormal)...the NPE_StreamAsFile function will
//  never be called in this case
//
NPError NP_LOADDS
NPP_NewStream(NPP instance,
              NPMIMEType type,
              NPStream *stream, 
              NPBool seekable,
              uint16 *stype)
{
	if(!instance)
	{
		return NPERR_INVALID_INSTANCE_ERROR;
	}

	// save the plugin instance object in the stream instance
	stream->pdata = instance->pdata;
	*stype = NP_ASFILE;
	return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_StreamAsFile
//
//	The stream is done transferring and here is a pointer to the file in the cache
//	This function is only called if the streamtype was set to NPAsFile.
//
void NP_LOADDS
NPP_StreamAsFile(NPP instance, NPStream *stream, const char* fname)
{
	if(fname == NULL || fname[0] == NULL)
	{
		return;
	}

/*	// get back the plugin instance object
    CPluginWindow * pluginData = (CPluginWindow *)instance->pdata;

	// get the avi object controller
    CAvi& aviPlayer = pluginData->GetAviStream();

	// open the avi driver with the specified name
	aviPlayer.Open(*pluginData, fname);
    aviPlayer.Update();

    // well, christ, the AVI window Update() paint doesn't work in Win95.
    // It works fine in NT and Win3.1, but Win95 is doing something
    // hostile.  So, I have a hack here that steps the frame forward
    // to paint the window;  barf ...

    // figure out whether to hack for Win 95
    DWORD dwVer = GetVersion();
    int iVer = (LOBYTE(LOWORD(dwVer))*100)+HIBYTE(LOWORD(dwVer));
    if(iVer > 394) {
        // Win 95
        aviPlayer.FrameForward();
    } */
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
//
//		These next 2 functions are really only directly relevant 
//		in a plug-in which handles the data in a streaming manner.  
//		For a NPAsFile stream, they are still called but can safely 
//		be ignored.
//
//		In a streaming plugin, all data handling would take place here...
//
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.

int32 STREAMBUFSIZE = 0X0FFFFFFF;   // we are reading from a file in NPAsFile mode
                                    // so we can take any size stream in our write
                                    // call (since we ignore it)
                                
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_WriteReady
//
//	The number of bytes that a plug-in is willing to accept in a subsequent
//	NPO_Write call.
//
int32 NP_LOADDS
NPP_WriteReady(NPP instance, NPStream *stream)
{
	return STREAMBUFSIZE;  
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_Write
//
//	Provides len bytes of data.
//
int32 NP_LOADDS
NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{   
	//MessageBox(NULL,"NPError NP_EXPORT NPE_Write()","Plug-in-test",MB_OK);
	return len;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_DestroyStream
//
//	Closes a stream object.  
//	reason indicates why the stream was closed.  Possible reasons are
//	that it was complete, because there was some error, or because 
//	the user aborted it.
//
NPError NP_LOADDS
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
	// because I am handling the stream as a file, I don't do anything here...
	// If I was streaming, I would know that I was done and do anything appropriate
	// to the end of the stream...   
	return NPERR_NO_ERROR;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\.
////\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//.
// NPP_Print
//
//	Printing the plugin (to be continued...)
//
void NP_LOADDS
NPP_Print(NPP instance, NPPrint* printInfo)
{
	if(printInfo == NULL)   // trap invalid parm
	{
		return;
	}

//    if (instance != NULL) {
//        CPluginWindow* pluginData = (CPluginWindow*) instance->pdata;
//		pluginData->Print(printInfo);
//	}
}

/*******************************************************************************
// NPP_URLNotify:
// Notifies the instance of the completion of a URL request. 
// 
// NPP_URLNotify is called when Netscape completes a NPN_GetURLNotify or
// NPN_PostURLNotify request, to inform the plug-in that the request,
// identified by url, has completed for the reason specified by reason. The most
// common reason code is NPRES_DONE, indicating simply that the request
// completed normally. Other possible reason codes are NPRES_USER_BREAK,
// indicating that the request was halted due to a user action (for example,
// clicking the "Stop" button), and NPRES_NETWORK_ERR, indicating that the
// request could not be completed (for example, because the URL could not be
// found). The complete list of reason codes is found in npapi.h. 
// 
// The parameter notifyData is the same plug-in-private value passed as an
// argument to the corresponding NPN_GetURLNotify or NPN_PostURLNotify
// call, and can be used by your plug-in to uniquely identify the request. 
 ******************************************************************************/

void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
}

