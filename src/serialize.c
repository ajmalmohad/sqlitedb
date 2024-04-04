#include "serialize.h"

/*
 * Serializes a row into a block of memory.
 *
 * Parameters:
 * - source: A pointer to the Row structure to be serialized.
 * - destination: A pointer to the block of memory where the serialized row will
 * be stored.
 *
 * The function works by copying each field from the source Row structure to the
 * correct location in the destination block of memory. The memcpy function is
 * used for the id field because it is an integer, and the strncpy function is
 * used for the username and email fields because they are strings.
 *
 * Does not return a value.
 */
void serialize_row(Row *source, void *destination) {
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
  strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

/*
 * Deserializes a row from a block of memory.
 *
 * Parameters:
 * - source: A pointer to the block of memory where the serialized row is
 * stored.
 * - destination: A pointer to the Row structure where the deserialized row will
 * be stored.
 *
 * The function works by copying each field from the source block of memory to
 * the correct location in the destination Row structure. The memcpy function is
 * used for all fields because we want to copy their exact binary
 * representations.
 *
 * Does not return a value.
 */
void deserialize_row(void *source, Row *destination) {
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}