#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#include <MinHook.h>

#include "args.h"

#pragma section(".text")
__declspec(allocate(".text")) const unsigned char stub[] = {
    0x48, 0x89, 0x4C, 0x24, 0x08,
    0x48, 0x89, 0x54, 0x24, 0x10,
    0x4C, 0x89, 0x44, 0x24, 0x18,
    0x4C, 0x89, 0x4C, 0x24, 0x20,
    0x48, 0x83, 0xEC, 0x48,
    0xF2, 0x0F, 0x11, 0x44, 0x24, 0x28,
    0xF2, 0x0F, 0x11, 0x4C, 0x24, 0x30,
    0xF2, 0x0F, 0x11, 0x54, 0x24, 0x38,
    0xF2, 0x0F, 0x11, 0x5C, 0x24, 0x40,
    0x48, 0x8D, 0x4C, 0x24, 0x28,
    0xFF, 0x15, 0x02, 0x00, 0x00, 0x00,
    0xEB, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF2, 0x0F, 0x10, 0x5C, 0x24, 0x40,
    0xF2, 0x0F, 0x10, 0x54, 0x24, 0x38,
    0xF2, 0x0F, 0x10, 0x4C, 0x24, 0x30,
    0xF2, 0x0F, 0x10, 0x44, 0x24, 0x28,
    0x48, 0x83, 0xC4, 0x48,
    0x4C, 0x8B, 0x4C, 0x24, 0x20,
    0x4C, 0x8B, 0x44, 0x24, 0x18,
    0x48, 0x8B, 0x54, 0x24, 0x10,
    0x48, 0x8B, 0x4C, 0x24, 0x08,
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

EXTERN_C VOID tailStub();
EXTERN_C LPVOID pOriginal = NULL;

#pragma optimize("", off)
EXTERN_C __declspec(noinline) int myfunction(int a1, int a2, int a3, int a4, int a5)
{
    return a1 + a2 + a3 + a4 + a5;
}

__declspec(noinline) float mydoubletime(double a, float b)
{
    return (float)a - b;
}

EXTERN_C VOID pHookfunc(Arguments<int, 5>* args)
{
    printf("rcx value: %d\n", *args->get<int>(0));
    printf("rdx value: %d\n", *args->get<int>(1));
    printf("r8 value: %d\n", *args->get<int>(2));
    printf("r9 value: %d\n", *args->get<int>(3));
    printf("stack value: %d\n", *args->get<int>(4));

    printf("floating point value 1: %f\n", *args->get<float>(0));
    printf("floating point value 2: %f\n", *args->get<float>(1));

    *args->get<int>(0) = 9;

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

    printf("%f\n", mydoubletime(1337.1337, 1.f));

    mydoubletime(1337.1337, 1.f);

    result = myfunction(1, 2, 3, 4, 5);

    printf("hooked myfunction: %d\n", result);

    return 0;
}
#pragma optimize("", on)