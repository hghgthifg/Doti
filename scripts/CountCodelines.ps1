$doti_lines   = (Get-ChildItem ./doti -Recurse -Filter *.cppm | Select-String .).Count
$editor_lines = (Get-ChildItem ./editor -Recurse -Filter *.cppm | Select-String .).Count

$cpp_lines = $doti_lines + $editor_lines
$glsl_lines = (Get-ChildItem ./resource -Recurse -Filter *.glsl | Select-String .).Count
$total_lines = $cpp_lines + $glsl_lines

Write-Output "Type[cpp]: $cpp_lines (wrapper not included)"
Write-Output "Type[glsl]: $glsl_lines"
Write-Output "Total: $total_lines"