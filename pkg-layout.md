# .PKG files layout

## types:
- `int` - `unsigned int` - 4 bytes
- `str` - `int length, char[length]` - 4 + length bytes 
- `entry` - `str file_path, int offset, int length` - 8 + length bytes
- `array[size, type]` - size * sizeof(type) bytes 

## layout:

```python
str header_version # looks like PKGVXXXX, latest PKGV0018
int length, array[length, entry] # array of entries
array[sum(entries[*].length), bytes[entry.length]] # array of data, referenced by offsets in entries, offsets starts from the begining of this array
```