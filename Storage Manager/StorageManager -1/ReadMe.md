
OUR IMPLEMENTATION

createPageFile 
This functions creates a new page file with '\0' bytes. 
We openthe file using the fopen() method in read and write mode., then set some memory for it using the malloc function.Next, memset() is used to fill a block of memory with '\0' bytes.

openPageFile 
This function opens the page file. If the file doesn't exist, return an error. We use fseek() to move the file pointer to the end of the file. Then using that, it calculates the total number of pages. Fields of the file handle are initialized using the arrow operator.

closePageFile
This function closes the page file using the fclose(). It returns an error if the function doesn't exist.

destroyPageFile
This function destroys the page file using the inbuilt remove().

readBlock
This function opens the page file and read each block of the page file, and stores it in the page handle. The method reads the block at position pageNum from a file and stores its content in the memory pointedto by the memPage page handle. We use the fseek() and fread() with the required parameters, to implement readBlock() method. The current page position is initialised to the file handle.

getBlockPos
This function returns the current position of the file from the file handle using the data structure, fHandle and stores it in the curPagePos variable

readFirstBlock
Reads the first block in the file from the file handle to the memPage page,Makes use of another function called readBlock to read the first page in the file.The readBlock function is called using 0 as a parameter for the first block in the file.Throws an error if the first block is not found in the file handle

readPreviousBlock
Reads the previous block in the file from the file handle to the memPage page.Checks if the previous block is found in the file handle.If it is not found, returns an error of non-existing page.If it is found, the current position of the file is retrieved from the data structure fHandle and stored in
current_pg_num variable.The readBlock function is called using current_pg_num -1 as the parameter to read the previous page
in the file.

readCurrentBlock
Reads the current page in the file from the file handle to the memPage page.If the current block is not found in the file handle, returns an error of non-existing page.If the current block is found, the readBlock function is called using current_page as the parameter.The current_page variable stores the current block position from the fHandle data structure.

readNextBlock

Reads the next block in the file from the file handle to the memPage.If the block is found in the file handle, the readBlock function is called using current_pg_number +1
as the parameter.The current_pg_number variable stores the current block position from the file handle.If the block is not found, returns an error of non-existing page

readLastBlock
read the last block, reuse the readBlock function , use 'lastPageNum = fHandle->totalNumPages - 1' to get the last page number and output lastPageNum


writeBlock

make sure the page can be write, use fileTag to point the file and ensure wether the requirements are feasible, write data and calculate the value and save the current pageNumber


writeCurrentBlock

if the current page number <0 then fail to write and output RC_WRITE_FAILED, reuse the writeblock funtion

appendEmptyBlock

ensure the fHandle is empty or not. If it is empty return RC_FILE_HANDLE_NOT_INIT,set an empty page ,set a pointer to locate the last page, determine the return of the pointer and output the code , put empty page into the file and calculate the size, determine the return of the pagesize output and free the memory





Use the makefile to execute the code.




