
#include <stdio.h>
#include <uzi/crt.h>
#include <uzi/windows.h>
#include <uzi/interlocked.h>

typedef struct _LIST_ITEM
{
	UZI_LIST_ENTRY ItemEntry;
	ULONG Signature;
} LIST_ITEM, *PLIST_ITEM;

int TestInterlockedDList(int argc, char* argv[])
{
	ULONG Count;
	PLIST_ITEM pListItem;
	UZI_PLIST_ENTRY pListHead;
	UZI_PLIST_ENTRY pListEntry;

	pListHead = (UZI_PLIST_ENTRY) _aligned_malloc(sizeof(UZI_LIST_ENTRY), MEMORY_ALLOCATION_ALIGNMENT);

	if (!pListHead)
	{
		printf("Memory allocation failed.\n");
		return -1;
	}

	InitializeListHead(pListHead);

	if (!IsListEmpty(pListHead))
	{
		printf("Expected empty list\n");
		return -1;
	}

	/* InsertHeadList / RemoveHeadList */

	printf("InsertHeadList / RemoveHeadList\n");

	for (Count = 1; Count <= 10; Count += 1)
	{
		pListItem = (PLIST_ITEM) _aligned_malloc(sizeof(LIST_ITEM), MEMORY_ALLOCATION_ALIGNMENT);
		pListItem->Signature = Count;
		InsertHeadList(pListHead, &(pListItem->ItemEntry));
	}

	for (Count = 10; Count >= 1; Count -= 1)
	{
		pListEntry = RemoveHeadList(pListHead);
		pListItem = (PLIST_ITEM) pListEntry;
		_aligned_free(pListItem);
	}

	/* InsertTailList / RemoveTailList */

	printf("InsertTailList / RemoveTailList\n");

	for (Count = 1; Count <= 10; Count += 1)
	{
		pListItem = (PLIST_ITEM) _aligned_malloc(sizeof(LIST_ITEM), MEMORY_ALLOCATION_ALIGNMENT);
		pListItem->Signature = Count;
		InsertTailList(pListHead, &(pListItem->ItemEntry));
	}

	for (Count = 10; Count >= 1; Count -= 1)
	{
		pListEntry = RemoveTailList(pListHead);
		pListItem = (PLIST_ITEM) pListEntry;
		_aligned_free(pListItem);
	}

	_aligned_free(pListHead);

	return 0;
}
