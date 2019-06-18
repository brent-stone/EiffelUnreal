# EiffelUnreal
Proof of concept for hooking Unreal Engine 4 using Eiffel via C++, sockets, and manual/implied serialization to UTF8.

A sphere will transition between a Red and Blue material every 6 seconds or when calling ChangeColor() in the sphere's ColorChangePOC component.

The actual material assignment is made using the sphere's Blueprint which is triggered by the ColorChangePOC's C++ OnBlue() and OnRed() BlueprintAssignable event broadcasts.

# Future Work
At some point in the future I'll be posting a sister repository with an expanded POC using Google's protocol buffers and gRPC. The goal will be a server running Eiffel managing complex communication between multiple Unreal clients.
