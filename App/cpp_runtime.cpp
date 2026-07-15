extern "C" void __cxa_pure_virtual() {
	while(1) {}
}

void operator delete(void* ptr) {
	while(1) {}
}

void operator delete[](void* ptr) {
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