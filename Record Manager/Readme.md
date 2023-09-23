FUNCTIONS AND IMPLEMENTATION

initRecordManager ( ) 

The task of initRecordManager function is to initialize the Record Manager and it calls initStorageManager to initialize the Record Manager

shutdownRecordManager( ) 

This function frees up memory of the record manager.After freeing the memory, the shutdownRecordManager function completely destroys the record
manager

createTable( ) 
The createTable function involves creating a new table, opening the newly created table, writing to
the new table followed by closing the table.Involves initializing the buffer pool followed by allocating memory to the record manager structure. Makes use of createPageFile function to create a table and openPageFile to open the newly created
table.Also makes use of writeBlock and closePageFile functions to write to the block followed by closing
the file

openTable( ) 

The openTable function is responsible for opening the table with the name ‘name’.Involves calling the PageHandle function.openTable function uses unpinPage function from the buffer manager to remove the page from the
buffer pool.After removing the page from the buffer pool, the page is written to disk using the forcePage
function from buffer manager

closeTable( ) 

The closeTable function closes the table referenced by the variable rel.Stores the table’s metadata before completely closing it.After storing the metadata, the function closeTable shuts down the Buffer Pool.Also frees the parameter rel

deleteTable( ) 

Deletes the table with the name ‘name’.Makes use of destroyPageFile from the storage manager to delete table

getNumTuples( ) 

Uses the reference variable rel to return the number of tuples / records present in the table

insertRecord( ) 

In order for the Buffer Manager to write the page's content back to the disk, we flag the page as dirty. Using the memcpy() C method, we copy the record's data (provided through parameter "record") into the new record before unpinning the page. The record is embedded in the table using a function, and its Record ID is set.

deleteRecord( ) 

Utilizing the Record ID and table name references listed in the capacity boundaries, the function is used to delete the record from the table. We unpin the page after marking it dirty so that the Buffer Manager can save its contents back to disk.

updateRecord( ) 

This function updates a record referenced by the parameter "record" in the table referenced by the parameter "rel". Using the meta-data of the table, it locates the page where the record is stored and pins that page in the buffer pool.
It sets the Record ID and navigates to the directory containing the record's data.
getRecord( ) 

This function fetches a record from the table that is referred by the parameter "rel" and has the Record ID "id" supplied in the parameter.It copies the data and sets the Record ID of the "record" argument to the ID of the record that is present on the page.


startScan( ) 

The RM ScanHandle data structure, which is supplied as an input to startScan(), is used by this function to begin a scan.

next( ) 

This function returns the tuple that meets the requirement 
We return error code RC_RM_NO_MORE_TUPLES if none of the tuples satisfy the requirement. 

closeScan( ) 

By looking at the scanCount value in the table's metadata, we determine whether the scan was complete or not. If it is higher than 0, the scan was not fully completed.


getRecordSize
 deal with schema, get the size of record and return the size


createSchema( ) 

When global variables are not allocated to available memory, allocate first,Schema get the value: numAttr/attrNames/dataTypes/typeLength/keys/keySize and finally return schema


createRecord( ) 

This function sets the pointer and allocate memory for schema/record, get the size of curent record, allocate space for record and initialize the page and slot


freeRecord( ) 

This capacity de-dispenses the memory space designated to the 'record' went through the boundary. 

getAttr( ) 

It allocates space for the Value data, save the data type, use for function to count the length and add it into the offset and get the attribute's data type and value of the '*value' parameter

setAttr( ) 

It allocates space for the Value data and identifiy the data type, and uses parameter pass the datasen record, schema and attribute number and sets the attribute value


HOW TO RUN:

1) Move to project directory in Linux server

2) make clean

4) make

5) ./test_assign3


