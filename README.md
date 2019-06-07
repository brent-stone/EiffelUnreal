# EiffelUnreal
Proof of concept for hooking Unreal Engine 4 using Eiffel via C++

A sphere will transition between a Red and Blue material every 6 seconds or when calling ChangeColor() in the sphere's ColorChangePOC component.

The actual material assignment is made using the sphere's Blueprint which is triggered by the ColorChangePOC's C++ OnBlue() and OnRed() BlueprintAssignable event broadcasts.
