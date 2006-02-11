/*****************************************************************
|
|      File: BltByteStreamUser.h
|
|      BlueTune - InputStreamUser & OutputStreamUser
|
|      (c) 2002-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _BLT_BYTE_STREAM_USER_H_
#define _BLT_BYTE_STREAM_USER_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Atomix.h"
#include "BltTypes.h"
#include "BltMedia.h"

/*----------------------------------------------------------------------
|       BLT_InputStreamUser
+---------------------------------------------------------------------*/
ATX_DECLARE_INTERFACE(BLT_InputStreamUser)
ATX_BEGIN_INTERFACE_DEFINITION(BLT_InputStreamUser)
    BLT_Result (*SetStream)(BLT_InputStreamUserInstance* instance,
                            ATX_InputStream*             stream,
                            const BLT_MediaType*         media_type);
ATX_END_INTERFACE_DEFINITION(BLT_ByteStreamUser)

/*----------------------------------------------------------------------
|       convenience macros
+---------------------------------------------------------------------*/
#define BLT_InputStreamUser_SetStream(object, stream, media_type) \
ATX_INTERFACE(object)->SetStream(ATX_INSTANCE(object), stream, media_type)

/*----------------------------------------------------------------------
|       BLT_OutputStreamUser
+---------------------------------------------------------------------*/
ATX_DECLARE_INTERFACE(BLT_OutputStreamUser)
ATX_BEGIN_INTERFACE_DEFINITION(BLT_OutputStreamUser)
    BLT_Result (*SetStream)(BLT_OutputStreamUserInstance* instance,
                            ATX_OutputStream*             stream);
ATX_END_INTERFACE_DEFINITION(BLT_ByteStreamUser)

/*----------------------------------------------------------------------
|       convenience macros
+---------------------------------------------------------------------*/
#define BLT_OutputStreamUser_SetStream(object, stream) \
ATX_INTERFACE(object)->SetStream(ATX_INSTANCE(object), stream)

#endif /* _BLT_BYTE_STREAM_USER_H_ */