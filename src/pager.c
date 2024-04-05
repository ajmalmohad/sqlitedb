#include "pager.h"

/*
 * Retrieves a page from the pager.
 *
 * Parameters:
 * - pager: A pointer to the Pager structure.
 * - page_num: The number of the page to be retrieved.
 *
 * The function first checks if the requested page number is within bounds. If
 * it's not, the function prints an error message and exits.
 *
 * If the requested page is not in the pager's cache (i.e., it's a cache miss),
 * the function allocates memory for the page and checks if the page exists in
 * the file. If it does, the function reads the page from the file into the
 * newly allocated memory.
 *
 * The function then stores a pointer to the page in the pager's cache and
 * updates the number of pages in the pager if necessary.
 *
 * Returns a pointer to the requested page.
 */
void *get_page(Pager *pager, uint32_t page_num) {
  if (page_num >= TABLE_MAX_PAGES) {
    printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
           TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }

  if (pager->pages[page_num] == NULL) {
    // Cache miss. Allocate memory and load from file.
    void *page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;

    // We might save a partial page at the end of the file
    if (pager->file_length % PAGE_SIZE) {
      num_pages += 1;
    }

    if (page_num < num_pages) {
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1) {
        printf("Error reading file: %d\n", errno);
        exit(EXIT_FAILURE);
      }
    }

    pager->pages[page_num] = page;

    if (page_num >= pager->num_pages) {
      pager->num_pages = page_num + 1;
    }
  }

  return pager->pages[page_num];
}

/*
 * Opens a pager for a database file.
 *
 * Parameters:
 * - filename: The name of the file to be opened.
 *
 * The function opens the file in read/write mode, creating it if it doesn't
 * exist. It then seeks to the end of the file to determine its length.
 *
 * A new Pager structure is allocated and its fields are initialized with the
 * file descriptor, the file length, and the number of pages in the file
 * (calculated as the file length divided by the page size).
 *
 * If the file length is not a whole number of pages, the function prints an
 * error message and exits, as this indicates a corrupt file.
 *
 * The function then initializes each page in the pager's cache to NULL.
 *
 * Returns a pointer to the new Pager structure.
 */
Pager *pager_open(const char *filename) {
  int fd = open(filename,
                O_RDWR |     // Read/Write mode
                    O_CREAT, // Create file if it does not exist
                S_IWUSR |    // User write permission
                    S_IRUSR  // User read permission
  );

  if (fd == -1) {
    printf("Unable to open file\n");
    exit(EXIT_FAILURE);
  }

  off_t file_length = lseek(fd, 0, SEEK_END);

  Pager *pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->num_pages = (file_length / PAGE_SIZE);

  if (file_length % PAGE_SIZE != 0) {
    printf("Db file is not a whole number of pages. Corrupt file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    pager->pages[i] = NULL;
  }

  return pager;
}

/*
 * Writes a page from the pager's cache to the database file.
 *
 * Parameters:
 * - pager: A pointer to the Pager structure.
 * - page_num: The number of the page to be written.
 *
 * The function first checks if the specified page is in the pager's cache. If
 * it's not, the function prints an error message and exits.
 *
 * The function then seeks to the correct position in the file for the specified
 * page. If the seek operation fails, the function prints an error message and
 * exits.
 *
 * The function then writes the page from the pager's cache to the file.
 * If the write operation fails, the function prints an error message and exits.
 *
 * Does not return a value.
 */
void pager_flush(Pager *pager, uint32_t page_num) {
  if (pager->pages[page_num] == NULL) {
    printf("Tried to flush null page\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

  if (offset == -1) {
    printf("Error seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written =
      write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

  if (bytes_written == -1) {
    printf("Error writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

/*
Until we start recycling free pages, new pages will always
go onto the end of the database file
*/
uint32_t get_unused_page_num(Pager *pager) { return pager->num_pages; }