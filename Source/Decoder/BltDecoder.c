/*****************************************************************
|
|      File: BltDecoder.c
|
|      BlueTune - Sync Layer
|
|      (c) 2002-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|    includes
+---------------------------------------------------------------------*/
#include "BltDecoder.h"
#include "BltCore.h"
#include "BltStream.h"
#include "BltBuiltins.h"
#include "BltDebug.h"

/*----------------------------------------------------------------------
|    types
+---------------------------------------------------------------------*/
struct BLT_Decoder {
    BLT_Core          core;
    BLT_Stream        stream;
    BLT_DecoderStatus status;
};

/*----------------------------------------------------------------------
|    BLT_Decoder_Create
+---------------------------------------------------------------------*/
BLT_Result 
BLT_Decoder_Create(BLT_Decoder** decoder)
{
    BLT_Result result;

    /* allocate a new decoder object */
    *decoder = (BLT_Decoder*)ATX_AllocateZeroMemory(sizeof(BLT_Decoder));
    if (*decoder == NULL) {
        return BLT_ERROR_OUT_OF_MEMORY;
    }

    /* get the core object */
    result = BLT_Init(&(*decoder)->core);
    if (BLT_FAILED(result)) goto failed;

    /* create a stream */
    result = BLT_Core_CreateStream(&(*decoder)->core, &(*decoder)->stream);
    if (BLT_FAILED(result)) goto failed;

    /* done */
    return BLT_SUCCESS;

 failed:
    BLT_Decoder_Destroy(*decoder);
    return result;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_Destroy
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_Destroy(BLT_Decoder* decoder)
{
    ATX_RELEASE_OBJECT(&decoder->stream);
    BLT_Core_Destroy(&decoder->core);
    BLT_Terminate();
    ATX_FreeMemory(decoder);

    return BLT_SUCCESS;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_RegisterBuiltins
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_RegisterBuiltins(BLT_Decoder* decoder)
{
    return BLT_Builtins_RegisterModules(&decoder->core);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_RegisterBuiltins
+---------------------------------------------------------------------*/
BLT_Result 
BLT_Decoder_RegisterModule(BLT_Decoder* decoder, const BLT_Module* module)
{
    return BLT_Core_RegisterModule(&decoder->core, module);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_ClearStatus
+---------------------------------------------------------------------*/
static BLT_Result
BLT_Decoder_ClearStatus(BLT_Decoder* decoder) 
{
    ATX_SetMemory(&decoder->status, 0, sizeof(decoder->status));
    return BLT_SUCCESS;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_UpdateStatus
+---------------------------------------------------------------------*/
static BLT_Result
BLT_Decoder_UpdateStatus(BLT_Decoder* decoder) 
{
    BLT_StreamStatus status;
    BLT_Result       result;

    result = BLT_Stream_GetStatus(&decoder->stream, &status);
    if (BLT_SUCCEEDED(result)) {
        decoder->status.time_stamp = status.output_status.time_stamp;
        decoder->status.position   = status.position;
        /*BLT_Debug("+++++++++++++ %d.%09d : %d/%d\n",
                  decoder->status.time_stamp.seconds,
                  decoder->status.time_stamp.nanoseconds,
                  decoder->status.position.offset,
                  decoder->status.position.range);*/
    }

    return BLT_SUCCESS;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_GetSettings
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_GetSettings(BLT_Decoder* decoder, ATX_Properties* settings) 
{
    return BLT_Core_GetSettings(&decoder->core, settings);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_GetStatus
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_GetStatus(BLT_Decoder* decoder, BLT_DecoderStatus* status) 
{
    /* update the status cache */
    BLT_Decoder_UpdateStatus(decoder);

    /* return the cached info */
    *status = decoder->status;

    return BLT_SUCCESS;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_GetStreamProperties
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_GetStreamProperties(BLT_Decoder*    decoder, 
                                ATX_Properties* properties) 
{
    return BLT_Stream_GetProperties(&decoder->stream, properties);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_SetEventListener
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_SetEventListener(BLT_Decoder*             decoder, 
                             const BLT_EventListener* listener)
{
    /* set the listener of the stream */
    return BLT_Stream_SetEventListener(&decoder->stream, listener);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_SetInput
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_SetInput(BLT_Decoder* decoder, BLT_CString name, BLT_CString type)
{
    /* clear the status */
    BLT_Decoder_ClearStatus(decoder);

    if (name == NULL || name[0] == '\0') {
        /* if the name is NULL or empty, it means reset */
        return BLT_Stream_ResetInput(&decoder->stream);
    } else {
        /* set the input of the stream by name */
        return BLT_Stream_SetInput(&decoder->stream, name, type);
    }
}

/*----------------------------------------------------------------------
|    BLT_Decoder_SetOutput
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_SetOutput(BLT_Decoder* decoder, BLT_CString name, BLT_CString type)
{
    if (name == NULL || name[0] == '\0') {
        /* if the name is NULL or empty, it means reset */
        return BLT_Stream_ResetOutput(&decoder->stream);
    } else {
        if (ATX_StringsEqual(name, BLT_DECODER_DEFAULT_OUTPUT_NAME)) {
	    /* if the name is BLT_DECODER_DEFAULT_OUTPUT_NAME, use default */ 
  	    return BLT_Stream_SetOutput(&decoder->stream, NULL, type);
	} else {
            /* set the output of the stream by name */
            return BLT_Stream_SetOutput(&decoder->stream, name, type);
	}
    }
}

/*----------------------------------------------------------------------
|    BLT_Decoder_AddNodeByName
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_AddNodeByName(BLT_Decoder*   decoder, 
                          BLT_MediaNode* where, 
                          BLT_CString    name)
{
    return BLT_Stream_AddNodeByName(&decoder->stream, where, name);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_PumpPacket
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_PumpPacket(BLT_Decoder* decoder)
{
    BLT_Result result;

    /* pump a packet */
    result = BLT_Stream_PumpPacket(&decoder->stream);
    if (BLT_FAILED(result)) return result;

    return BLT_SUCCESS;
}

/*----------------------------------------------------------------------
|    BLT_Decoder_Stop
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_Stop(BLT_Decoder* decoder)
{
    /* stop the stream */
    return BLT_Stream_Stop(&decoder->stream);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_Pause
+---------------------------------------------------------------------*/
BLT_Result
BLT_Decoder_Pause(BLT_Decoder* decoder)
{
    /* pause the stream */
    return BLT_Stream_Pause(&decoder->stream);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_SeekToTime
+---------------------------------------------------------------------*/
BLT_Result 
BLT_Decoder_SeekToTime(BLT_Decoder* decoder, BLT_Cardinal time)
{
    return BLT_Stream_SeekToTime(&decoder->stream, time);
}

/*----------------------------------------------------------------------
|    BLT_Decoder_SeekToPosition
+---------------------------------------------------------------------*/
BLT_Result 
BLT_Decoder_SeekToPosition(BLT_Decoder* decoder,
                           BLT_Size     offset,
                           BLT_Size     range)
{
    return BLT_Stream_SeekToPosition(&decoder->stream, offset, range);
}

