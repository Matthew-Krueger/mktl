// File: Memory.c
// Description: C89 compatible in-code memory sanitizer. This only takes care of a pAllocator, NOT any C++ extensions
//                 of this file.
// Author: Matthew Krueger <mckrueg@bgsu.edu>
// Course: CS 3350, fall 2024

#include <mktl_c/Memory.h>
#include <stdio.h>

struct Node {
    void* ptr;
    struct PointerInfo structPointerInfo;
    struct Node* next;
} ;

__MKTL_API_HIDDEN static char registeredFlag = 0;
__MKTL_API_HIDDEN static struct Node* memoryInfo = NULL;
__MKTL_API_HIDDEN static unsigned long long bytesAllocated = 0;
__MKTL_API_HIDDEN static unsigned long long allocationCount = 0;
__MKTL_API_HIDDEN static unsigned long long bytesDeallocated = 0;
__MKTL_API_HIDDEN static unsigned long long deallocationCount = 0;

__MKTL_API_HIDDEN void memorySanAtExitHook(){

    // clear linked list, and print if there's an issue
    while(memoryInfo){
        struct Node* temp = memoryInfo;
        memoryInfo = temp->next;

        // if pointer is invalid or allocated, there's a memory issue and we should alert to this condition
        if(temp->structPointerInfo.enumPointerState == INVALID){
            fprintf(stderr, "Pointer %p of size %lu is in an invalid state at application close.\n", temp->ptr, temp->structPointerInfo.pointerSize);
        }
        if(temp->structPointerInfo.enumPointerState == ALLOCATED){
            fprintf(stderr, "Pointer %p of size %lu is still in allocated state at application close.\n", temp->ptr, temp->structPointerInfo.pointerSize);
        }

        // it is not the job of this function to deallocate the pointers. We're just going to delete our reference since
        // this function should only be called at exit
        free(temp);
    }

}

/**
 * Registers a callback at exit on the first call (otherwise nothing) to clear the linked list
 */
__MKTL_API_HIDDEN void memorySanRegisterSystem(){
    if(registeredFlag) return;
    atexit(memorySanAtExitHook);
    registeredFlag = 1;
}

/**
 * Adds a node to the linked list
 * @param head the head pointer (just in case you have multiple pAllocator)
 * @param ptr the pointer to add
 * @param structPointerInfo The Info about the pointer
 */
__MKTL_API_HIDDEN void memorySanAddNode(struct Node** head, void* ptr, struct PointerInfo structPointerInfo){

    struct Node* newNode = malloc(sizeof(struct Node));
    if(!newNode){
        fprintf(stderr, "Cannot allocate new node to track memory allocations.\n");
        return;
    }

    newNode->ptr = ptr;
    newNode->structPointerInfo = structPointerInfo;
    newNode->next = *head;
    *head = newNode;
}

/**
 * Find according to linus torvalds talk about good code taste
 * @param head the head pointer
 * @param ptr the pointer to find the data about
 * @return the indirect pointer to the node that is about the given pointer
 */
__MKTL_API_HIDDEN struct Node** memorySanFindIndirect(struct Node** head, void* ptr){

    if(!ptr){
        fprintf(stderr, "Cannot find pointer in memory allocation tracking list, as no pointer was given.\n");
        return NULL;
    }

    if(!head){
        fprintf(stderr, "Cannot find pointer in memory allocation tracking list, as list is empty.\n");
        return NULL;
    }

    struct Node** indirect = &(*head);
    while(indirect && (*indirect) && (*indirect)->ptr != ptr){
        indirect = &(*indirect)->next;
    }

    return indirect;

}

/**
 * Remove a node from the list.
 * @param head The head pointer
 * @param ptr The pointer *about which* to remove the data
 */
__MKTL_API_HIDDEN void memorySanRemoveNode(struct Node** head, void* ptr){

    if(!ptr){
        fprintf(stderr, "Cannot remove pointer from memory allocation tracking list, as no pointer was given.\n");
        return;
    }

    if(!head){
        fprintf(stderr, "Cannot remove pointer from memory allocation tracking list, as list is empty.\n");
        return;
    }

    struct Node** indirect = memorySanFindIndirect(head, ptr);

    // If the node was not found
    if (!indirect || !*indirect) {
        fprintf(stderr, "Node to delete not found or invalid.\n");
        return;
    }


    // We have a valid node to delete
    struct Node* node_to_delete = *indirect;

    // Relink the list: bypass the node to delete
    *indirect = node_to_delete->next;

    // Free the memory associated with the node
    free(node_to_delete);

}

/**
 * Get the pointer info. This should not be called publicly since it does NOT copy and instead returns our internal
 * memory from our data structure.
 * @param pVoid The pointer about which to find.
 * @return
 */
__MKTL_API_HIDDEN struct PointerInfo* memorySanGetPointerInfoInternal(void *pVoid){

    struct Node** indirect = memorySanFindIndirect(&memoryInfo, pVoid);
    if(!indirect) return NULL; // guard

    struct Node* structNode = *indirect;
    if(!structNode) return NULL; // guard

    return &structNode->structPointerInfo;

}

void *trackedMalloc(unsigned long bytes){

    memorySanRegisterSystem();

    // initialize the tracking struct
    struct PointerInfo structPointerInfo;
    structPointerInfo.enumPointerState = INVALID;
    structPointerInfo.pointerSize = bytes;

    // allocate the real pointer
    void* trackedPtr = malloc(bytes);
    structPointerInfo.enumPointerState = ALLOCATED;

    ++allocationCount;
    bytesAllocated+=bytes;

    memorySanAddNode(&memoryInfo, trackedPtr, structPointerInfo);

    return trackedPtr;

}

void trackedFree(void *pVoid){

    struct PointerInfo* pStructPointerInfo = memorySanGetPointerInfoInternal(pVoid);
    if(pStructPointerInfo){
        pStructPointerInfo->enumPointerState = DEALLOCATED;
        ++deallocationCount;
        bytesDeallocated += pStructPointerInfo->pointerSize;
    }

    free(pVoid);

}

struct PointerInfo getPointerInfo(void *pVoid){
    struct PointerInfo* pStructPointerInfo = memorySanGetPointerInfoInternal(pVoid);
    if(!pStructPointerInfo){
        fprintf(stderr, "Attempting to get pointer info of non-tracked pointer\n");
        struct PointerInfo structPointerInfo;
        structPointerInfo.enumPointerState = INVALID;
        structPointerInfo.pointerSize = 0;
        return structPointerInfo;
    }

    return *pStructPointerInfo;
}

unsigned long long getTotalBytesAllocated(){
    return bytesAllocated;
}

unsigned long long getAllocationCount(){
    return allocationCount;
}

unsigned long long getTotalBytesDeallocated(){
    return bytesDeallocated;
}

unsigned long long getDeallocationCount(){
    return deallocationCount;
}

unsigned long long getBytesCurrentlyAllocated(){
    return getTotalBytesAllocated() - getTotalBytesDeallocated();
}
