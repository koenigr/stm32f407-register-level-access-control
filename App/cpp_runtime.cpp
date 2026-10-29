// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp) -- ABI-required name
extern "C" void __cxa_pure_virtual() {
	while(1) {}
}

// NOLINTNEXTLINE(cert-dcl54-cpp) -- no heap on target, delete is a stub
void operator delete(void*) {
	while(1) {}
}

// NOLINTNEXTLINE(cert-dcl54-cpp) -- no heap on target, delete is a stub
void operator delete[](void*) {
	while(1) {}
}

void operator delete(void*, unsigned int)
{
	while(1) {}
}


void operator delete[](void*, unsigned int)
{
	while(1) {}
}