# README

## Usage
need to start RTSS.exe first

```
Usage:
-p processName.exe (case Sensitive)
-o filename.csv (default stdout)
-t 0 (runtime seconds, defalut=0 means no limit)
-s close stdout (default true)
```

## Example
. .\RTSSTest.exe -p FurMark.exe -o a.csv -S -t 3

## Tips

- when ms build, choose x86, not x64
- Multiple processes with the same name can be watched
- Process name case sensitive
- The csv file is written streaming
