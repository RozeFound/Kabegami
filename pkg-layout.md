# Documenting .PKG files layout

## types:
- `uint` - `unsigned int` - 4 bytes
- `string` - `uint length, char[length]` - 4 + length bytes 
- `entry` - `string file_path, uint offset, uint length` - 8 + length bytes
- `array[size, type]` - size * sizeof(type) bytes 

## layout:

`start of file`

`string header_version` - looks like PKGVXXXX, latest PKGV0018

`uint length, array[length, entry]` - array of entries

`array[sum(entries[*].length), bytes[entry.length]]` - array of data, referenced by offsets in entries, offsets starts from the begining of this array

`end of file`