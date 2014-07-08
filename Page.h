
#ifndef _PAGE_H_
#define _PAGE_H_

struct Page
{
    unsigned int ProcessNumber;
    unsigned int PageNumber;
    unsigned int Size;
    bool Occupied;

    Page();

    ~Page();

    bool isOccupied() const;
};

#endif
