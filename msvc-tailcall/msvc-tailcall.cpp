// msvc-tailcall.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <intrin.h>

#include <MinHook.h>

__declspec(noinline) int donothing(int a1)
{
    return a1 + 1;
}

EXTERN_C __declspec(noinline) int myfunction(int a1, int a2, int a3, int a4, int a5)
{
    int p = donothing(a1);
    int c = donothing(p);

    p += donothing(a2);

    p -= donothing(a3);

    c += donothing(a4);

    return donothing(a5) + p;
}

EXTERN_C VOID tailStub();
EXTERN_C LPVOID pOriginal = NULL;

EXTERN_C VOID pHookfunc(int* rcx_ptr, int* rdx_ptr, int* r8_ptr, int* r9_ptr, void* rsp,
    void* ret_addr, int rcx, int rdx, int r8, int r9, int stack_arg1)
{
	printf("rcx ptr value: %d\n", *rcx_ptr);
	printf("rdx ptr value: %d\n", *rdx_ptr);
	printf("r8 ptr value: %d\n", *r8_ptr);
	printf("r9 ptr value: %d\n", *r9_ptr);

    *rcx_ptr = 9;
	
	printf("rsp: %p\n", rsp);
	printf("ret from rsp: %p\n", *(LPVOID*)rsp);
	
    printf("ret addr: %p\n", ret_addr);

    printf("rcx: %d\n", rcx);
    printf("rdx: %d\n", rdx);
    printf("r8: %d\n", r8);
    printf("r9: %d\n", r9);
    printf("stack arg1: %d\n", stack_arg1);
}

#pragma optimize( "", off )
int main()
{
    int result = myfunction(1, 2, 3, 4, 5);

    printf("myfunction: %d\n", result);

    if (MH_Initialize() != MH_OK)
    {
        printf("failed #1\n");
        return TRUE;
    }

    if (MH_CreateHook(myfunction, tailStub, &pOriginal) != MH_OK)
    {
        printf("failed #2\n");
        return TRUE;
    }

    if (MH_EnableHook(myfunction) != MH_OK)
    {
        printf("failed #3\n");
        return TRUE;
    }

    result = myfunction(1, 2, 3, 4, 5);

    printf("hooked myfunction: %d\n", result);

    return 0;
}
#pragma optimize( "", on )
