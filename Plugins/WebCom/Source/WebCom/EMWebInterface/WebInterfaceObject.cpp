// Copyright 2021 x.Lin. All Rights Reserved
#include "WebInterfaceObject.h"
#include "ComWebBrowser.h"
#include "WebInterfaceCallback.h"

void UWebInterfaceObject::Broadcast( const FString& Name, const FString& Data, const FString& Callback )
{
	if ( !MyInterface.IsValid() )
		return;

	if ( Callback.IsEmpty() )
		MyInterface->OnInterfaceEvent.Broadcast( FName( *Name ), FJsonLibraryValue::Parse( Data ), FWebInterfaceCallback() );
	else
		MyInterface->OnInterfaceEvent.Broadcast( FName( *Name ), FJsonLibraryValue::Parse( Data ), FWebInterfaceCallback( MyInterface, Callback ) );
}
