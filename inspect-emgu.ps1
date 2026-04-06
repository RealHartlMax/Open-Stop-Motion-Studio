$asm = [System.Reflection.Assembly]::LoadFrom('.\.artifacts\bin\Debug\net10.0\Emgu.CV.dll')
$type = $asm.GetType('Emgu.CV.VideoCapture')
$members = $type.GetMembers() | Where-Object { $_.MemberType -eq 'Property' -or $_.MemberType -eq 'Method' }
$members | Select-Object Name, MemberType | Sort-Object Name | Format-Table -AutoSize
