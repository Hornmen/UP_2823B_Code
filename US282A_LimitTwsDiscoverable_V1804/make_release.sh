#!/bin/sh
#仅供版本发布时使用

echo "cleanup object file"

cur_path=$PWD/
case_path=$cur_path/case/
ap_path=$cur_path/case/ap
psp_path=$cur_path/psp_rel/

rm -fr $case_path/tools/analyze_timestamp_tools
rm -fr $case_path/doc
rm -fr $psp_path/doc
rm -fr $ap_path/bluetooth_stack
rm -fr $ap_path/ud_stub
rm -fr $ap_path/uart_stub
rm -fr $ap_path/uart_stub_service
rm -fr $ap_path/ap_tktest
rm -fr $ap_path/ap_record
rm -fr $psp_path/bin_original
rm -fr $psp_path/tools/MPTools
rm -fr $psp_path/cfg/MakeFile
rm -fr $psp_path/cfg/dbt
rm -fr $psp_path/cfg/strip_lib
rm -fr $case_path/fwpkg/buildAFI.bat
rm -fr $case_path/drv/test_sppble
rm -fr $case_path/drv/bt_controller
rm -fr $case_path/tools/APK_APP
rm -fr $case_path/tools/Gen_buildfw
mv $case_path/cfg/Makefile_release $case_path/cfg/Makefile
echo "done"