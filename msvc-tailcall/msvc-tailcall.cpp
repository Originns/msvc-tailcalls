#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <intrin.h>
#include "args.h"

#include <MinHook.h>

EXTERN_C VOID tailStub();
EXTERN_C LPVOID pOriginal = NULL;

#pragma optimize("", off)
EXTERN_C __declspec(noinline) int myfunction(int a1, int a2, int a3, int a4, int a5)
{
    return a1 + a2 + a3 + a4 + a5;
}

EXTERN_C VOID pHookfunc(Arguments<5> *args)
{
    printf("rcx value: %d\n", *args->get<int>(0));
    printf("rdx value: %d\n", *args->get<int>(1));
    printf("r8 value: %d\n", *args->get<int>(2));
    printf("r9 value: %d\n", *args->get<int>(3));
    printf("stack value: %d\n", *args->get<int>(4));

    args->set(0, 9);

    printf("rcx new value: %d\n", *args->get<int>(0));
}

int main()
{
    int result = myfunction(1, 2, 3, 4, 5);

    printf("myfunction: %d\n", result);

    if (MH_Initialize() != MH_OK)
    {
        printf("failed #1\n");
        return 1;
    }

    if (MH_CreateHook(myfunction, tailStub, &pOriginal) != MH_OK)
    {
        printf("failed #2\n");
        return 1;
    }

    if (MH_EnableHook(myfunction) != MH_OK)
    {
        printf("failed #3\n");
        return 1;
    }

    result = myfunction(1, 2, 3, 4, 5);

    printf("hooked myfunction: %d\n", result);

    return 0;
}
#pragma optimize("", on)
