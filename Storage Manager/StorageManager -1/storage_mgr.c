#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "storage_mgr.h"

FILE *openFile;



extern void initStorageManager (void) {

	printf("Advanced Database Organisation - CS525 \n");
	printf("Storage Manager \n");
	printf("Assignment-1 \n");
	printf("A20517767 Madiha Sadaf \n");
	printf("A20519771 Yiqi Wang \n");
	printf("A20503938 - Sara Ataullah \n");
   

}

//creates a page file in read and write mode
RC createPageFile(char *fileName){
    
	openFile=fopen(fileName,"w+");
	int i;
	
    char *memory=(char*)malloc(PAGE_SIZE);
    
    if(openFile == NULL){
        return RC_FILE_NOT_FOUND;}
    
	
	else{

		//for (i = 0; i < PAGE_SIZE; i++){
    	//memcpy(memory, '\0', PAGE_SIZE);
		//}
        memset(memory, '\0', PAGE_SIZE); 
		fwrite(memory, sizeof(char), PAGE_SIZE, openFile);	
		free(memory);		
		fclose(openFile);			
		return RC_OK;

    }
}
//opens a page file,initializes fields of the file handle
    RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
	
	
	openFile = fopen(fileName, "r+");
	
	
	
	if (openFile == 0){
		return RC_FILE_NOT_FOUND;
	}
	
	else
	{
		fseek(openFile, 0, SEEK_END);
		
		int lastByte = ftell(openFile); 	
		
		int ComL = lastByte + 1;
		
		int totalNumPages = ComL / PAGE_SIZE; 
		
		fHandle->mgmtInfo=openFile;
		
		fHandle -> totalNumPages = totalNumPages;
		
		fHandle -> fileName= fileName;
		
		fHandle -> curPagePos = 0;
		
		
		rewind(openFile); 
		
		return RC_OK;
	}	
    }
	
	//This function closes the page file using the fclose().
    RC  closePageFile(SM_FileHandle *fHandle){
        fclose(openFile);
        fHandle=NULL;
        return RC_OK;
    }



//This function destroys the page file using the inbuilt remove().
	RC destroyPageFile(char *fileName){
    if (openFile==NULL){
        return RC_FILE_NOT_FOUND;
    }
    else{
    remove(fileName);
    return RC_OK;
}
}

//opens the page file and read each block of the page file, and stores it in the page handle.
	RC readBlock(int pageNum,SM_FileHandle *fHandle,SM_PageHandle memPage){
	
	if(fHandle->totalNumPages<pageNum){
		return RC_READ_NON_EXISTING_PAGE;

	}
	else if(pageNum<0){
		return RC_READ_NON_EXISTING_PAGE;

	}
	else{
		float read;
		
		fseek(openFile,pageNum*PAGE_SIZE, SEEK_SET);
		
		read=fread(memPage,sizeof(char),PAGE_SIZE,openFile);
		
		
		if(read>PAGE_SIZE){
			return RC_READ_NON_EXISTING_PAGE;

		}
		else if(read<PAGE_SIZE){
			return RC_READ_NON_EXISTING_PAGE;

		}
		
		fHandle->curPagePos=pageNum;
		return RC_OK;

		}
	}

// Reads the block position. Returns an error if a file is not found
extern int getBlockPos(SM_FileHandle *fHandle)
{
	// If the file handle is Null, the function returns File Not Found Error
	if (fHandle == NULL) 
	{
		return RC_FILE_NOT_FOUND;
	}

	// When the file handle is not Null, the function reads the current page position and stores it in the curPagePos variable
	else
	{
		return fHandle->curPagePos;
	}
}

// Reads the first block from the file handle to the memPage 
extern RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	/* If the file handle is not null, this method reads the first block using the function readBlock
	Also returns an OK message  */
	if (fHandle != NULL)
	{
		return readBlock(0, fHandle, memPage); //0 i.e. First Block
		return RC_OK;
	}

	// Returns an error if the page is not found 
	else
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
}
// Reads the previous block from the file handle to the memPage
extern RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	// If the file handle is Null, returns an error of Non Existing Page
	if (fHandle == NULL)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}

	/*If the file handle is not null, the method reads the position of file handle and stores it in curPagePos. 
	The variable current_pg_num stores the value of the current block position
	The roadblock function returns the previous block position*/
	else
	{
		int current_pg_num;
		current_pg_num = fHandle->curPagePos;
		return readBlock(current_pg_num - 1, fHandle, memPage); // current_pg_num - 1 i.e. previous block position
		return RC_OK;
	}
}
	
   
// Reads the position of the current block from file handle to the memPage and returns an error if the block is not found
extern RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	// If the file handle is null, returns an error file not found
	if (fHandle == NULL)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}

	/* If the file handle is not null, then the method reads the current block position from file handle and it is stored in current_page variable
	Also returns an OK message */
	else
	{
		int current_page;
		current_page = fHandle->curPagePos;
		return readBlock(current_page, fHandle, memPage);
		return RC_OK;
	}
}



// Reads the position of the Next Block from file handle to memPage
extern RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//If the file handle is null, returns a Non existing page error
	if (fHandle == NULL)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}

	/*If the file handle is not null, then current page number is fetched and stored in current_pg_number
	The readblock function is used to read the positon of the next block*/
	else
	{
		int current_pg_number;
		current_pg_number = fHandle->curPagePos;
		return readBlock(current_pg_number + 1, fHandle, memPage); // current_pg_number + 1 i.e. next block
		return RC_OK;
	}
}

/******************
@function brief   : 
		readLastBlock(): read the last block
		
		1. reuse the readBlock function
		2. use 'lastPageNum = fHandle->totalNumPages - 1' to get the last page number
		3. output lastPageNum

@
*****************/
extern RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {

	int lastPageNum = fHandle->totalNumPages - 1; // get the last page number
	return readBlock(lastPageNum, fHandle, memPage);

}

/******************
@function brief   :
		writeCurrentBlock():

		1. if the current page number <0 then fail to write and output RC_WRITE_FAILED
		2. reuse the writeblock funtion

*****************/

RC writeBlock(int PageNum_a, SM_FileHandle *fHandle, SM_PageHandle memPage) {

	if (PageNum_a < 0 || fHandle->totalNumPages < (PageNum_a + 1)) {
		return RC_READ_NON_EXISTING_PAGE;
	}

	long Offsett = PageNum_a * PAGE_SIZE * sizeof(char);

	int seekTag1 = fseek(fHandle->mgmtInfo, Offsett, SEEK_SET);

	if (seekTag1 != 0) {
		return RC_SEEK_POSITION_ERROR;
	}

	
	int writtenSize = fwrite(memPage, sizeof(char), PAGE_SIZE,
		fHandle->mgmtInfo); // return size

	if (writtenSize != PAGE_SIZE) {
		return RC_SEEK_POSITION_ERROR;
	}

	fHandle->curPagePos;
	fHandle = PageNum_a;

	return RC_OK;
}


/******************
@function brief   :
		writeCurrentBlock():

		1. if the current page number <0 then fail to write and output RC_WRITE_FAILED
		2. reuse the writeblock funtion

*****************/

extern RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	
	if (fHandle->curPagePos < 0 || fHandle == NULL) { //If curPagePos < 0 or empty, fail to write
		return RC_WRITE_FAILED;
	}

	return writeBlock(fHandle->curPagePos, fHandle, memPage); //reuese write block function
	 
}
/******************
@function brief   :
		appendEmptyBlock():

		1. ensure the fHandle is empty or not. If it is empty return RC_FILE_HANDLE_NOT_INIT
		2. set an empty page
		3. set a pointer to locate the last page
		4. determine the return of the pointer and output the code
		5. put empty page into the file and calculate the size
		6. determine the return of the pagesize output
		7. free the memory


*****************/
extern RC appendEmptyBlock(SM_FileHandle *fHandle) {
	
	openFile = fopen(fHandle->fileName, "a2+");

	if (openFile == NULL) { // if  it is empty return RC_FILE_HANDLE_NOT_INIT
		return RC_FILE_HANDLE_NOT_INIT;
	}

	char emptyPage = (char *)calloc(PAGE_SIZE, sizeof(char));// set an empty page and apply for the corresponding memory
	int openFile = fseek(openFile, 0, SEEK_END);//set a pointer to seek until the end page of the file
	RC rc = RC_WRITE_FAILED;

	if (openFile != 0) {
		rc = RC_WRITE_FAILED;
	}
	else
	{
		int pagesize = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fHandle->fileName);//write that empty page to the file and calculate the size
		if (pagesize != PAGE_SIZE) {
			rc = RC_WRITE_FAILED;
		}
		else {
			fHandle->curPagePos = fHandle->totalNumPages++;
			rc = RC_OK;
		}
		fclose(openFile);
	}
		
	free(emptyPage);//free memory
	return(rc);

	}
/******************
@function brief   :
		ensureCapacity():
		1. if the filename is empty output  RC_FILE_HANDLE_NOT_INIT
		2. ensure totalNumPages are more than it needed
		
*****************/

extern RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {

	if (fHandle->fileName == NULL) {//fileName is empty
		return RC_FILE_HANDLE_NOT_INIT;
	}
	if (fHandle->totalNumPages >= numberOfPages) {// ensure the feasiblity
		return RC_OK;
	}
	openFile = fopen(fHandle->fileName, "a3+");
	if (openFile == NULL) {
		return RC_FILE_NOT_FOUND;
	}
	while (numberOfPages > fHandle->totalNumPages)
		appendEmptyBlock(fHandle);
		fclose(openFile);
		return RC_OK;
	}

