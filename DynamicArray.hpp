#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <cstdlib>
#include "splashkit.h"

using std::to_string;

template <typename T>
struct dynamic_array
{
  T *data;
  unsigned int size;
  unsigned int capacity;
};

template <typename T>
dynamic_array<T> *create(unsigned int capacity = 50)
{
  dynamic_array<T> *arr = (dynamic_array<T> *)malloc(sizeof(dynamic_array<T>));
  if (!arr)
  {
    // Allocation failed, set capacity to 0 and return nullptr
    return nullptr;
  }
  arr->size = 0;
  arr->capacity = capacity;
  arr->data = (T *)malloc(sizeof(T) * capacity);
  if (!arr->data)
  {
    arr->capacity = 0;
    fre(arr);
    return nullptr;
  }

  // Call placement new for each element to invoke constructors
  for (unsigned int i = 0; i < capacity; ++i)
  {
    new (&arr->data[i]) T();
  }
  return arr;
}

template <typename T>
void fre(dynamic_array<T> *array)
{
  if (!array)
    return;
  // Call destructor for each element
  for (unsigned int i = 0; i < array->capacity; ++i)
  {
    array->data[i].~T();
  }

  array->size = 0;
  array->capacity = 0;

  free(array->data);
  free(array);
}

template <typename T>
bool resize(dynamic_array<T> *array, unsigned int new_capacity)
{
  if (!array)
    return false;
  if (new_capacity == array->capacity)
    return true;

  // If shrinking, call destructors on removed elements
  if (new_capacity < array->size)
  {
    for (unsigned int i = new_capacity; i < array->size; ++i)
    {
      array->data[i].~T();
    }
    array->size = new_capacity;
  }

  // Resize memory using realloc
  T *new_data = (T *)realloc(array->data, sizeof(T) * new_capacity);
  if (!new_data)
    return false;

  // Initialize new values
  for (unsigned int i = array->capacity; i < new_capacity; i++)
  {
    new (&new_data[i]) T();
  }

  array->data = new_data;
  array->capacity = new_capacity;

  return true;
}

template <typename T>
bool add(dynamic_array<T> *array, const T &value)
{
  if (!array)
    return false;

  // Double the capacity if full, or set to 1 if capacity is 0
  if (array->size >= array->capacity && !resize(array, array->capacity * 2 + 1))
  {
    return false;
  }

  array->data[array->size] = value;
  array->size++;
  return true;
}

template <typename T>

T get(const dynamic_array<T> *array, unsigned int index)
{
  if (!array)
    throw string("Array not allocated");
  if (index >= array->size)
    throw string("Index out of bounds");
  return array->data[index];
}

template <typename T>
void set(dynamic_array<T> *array, unsigned int index, const T &value)
{
  if (!array)
    throw string("Array not allocated");
  if (index >= array->size)
    throw string("Index out of bounds");
  array->data[index] = value;
}

#endif