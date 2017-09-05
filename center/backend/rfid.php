<?php
//get info
$id=$_GET["bus_id"];
$rfid=$_GET["rfid"];

//get data for checking whether the info had repeated or not
//rfid
$folder="rfid";
$file=date('ymdhi');
$path="https://bus-all.firebaseio.com/bus_rfid.json";
download($folder,$file,$path);
$data_content = file_get_contents("./bus/$folder/$file");
$rfid_data = json_decode($data_content,true);
/*Bus Event*/
$folder="Bus_Event";
$file=date('ymdhi');
$path="https://tcgbusfs.blob.core.windows.net/blobbus/GetBusEvent.gz";
download($folder,$file,$path);
$event_content = file_get_contents("./bus/$folder/$file");
$event = json_decode($event_content,true);
//looking for the path id for downloading
$event_no=0;
$path_id=0;
for(;$event_no<count($event["BusInfo"]);$event_no++){
    if($event["BusInfo"][$event_no]["CarID"]==$id){
        $path_id=$event["BusInfo"][$event_no]["RouteID"];
        break;
    }
}

//download stop_people
$folder="monitor_getinfo";
$file=date('ymdhi');
$path="https://bus-all.firebaseio.com/monitor_bus_info/$path_id/stations.json";
download($folder,$file,$path);
$people_content = file_get_contents("./bus/$folder/$file");
$people = json_decode($people_content,true);



//rfid
//check if the rfid had appeared
$isRepeated=false;
$whichOneOnBus = 0;
for($whichOneOnBus=0;$whichOneOnBus<count($rfid_data[$id]);$whichOneOnBus++){
    //if had, delete it
    if($rfid_data[$id][$whichOneOnBus]==$rfid){
        $base_path="https://bus-all.firebaseio.com/bus_rfid/$id/$whichOneOnBus.json";
        upload("DELETE",'',$base_path);
        $isRepeated=true;
        break;
    }
}
//if not, record it
if(!$isRepeated){
    $base_path="https://bus-all.firebaseio.com/bus_rfid/$id.json";
    $rfid_data=json_encode(array("$whichOneOnBus"=>$rfid));
    upload("PATCH",$rfid_data,$base_path);
    //station people --
    $stop_id=$event["BusInfo"][$event_no]["StopID"];
    $number=$people[$stop_id]["station_people"]-1;
    //上傳到主要資料
    $path="https://bus-all.firebaseio.com/monitor_bus_info/$path_id/stations/$stop_id.json";
    $rfid_data=json_encode(array("station_people"=>$number));
    upload("PATCH",$rfid_data,$path);
    //備份，讓主要資料更新時可以抓
    $path1="https://bus-all.firebaseio.com/people/$path_id/$stop_id.json";
    $rfid_data=json_encode(array("station_people"=>$number));
    upload("PATCH",$rfid_data,$path1);
}


function upload($method,$data,$base_path){
    //init curl
    $ch = curl_init();
    //set curl options 設定你要傳送參數的目的地檔案
    curl_setopt($ch, CURLOPT_URL, "$base_path");
    curl_setopt($ch, CURLOPT_HEADER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
    //開啓patch選項
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "$method");
    curl_setopt($ch, CURLOPT_POSTFIELDS,$data);
    //execute curl
    curl_exec($ch);
    curl_close($ch);
}


function download($folder,$file,$path){
    //check dir
    $dirname = "./bus/$folder";
    if (!file_exists($dirname)) {
        // Create nested folder
        mkdir($dirname, 0777, true);
        echo "The directory $dirname was successfully created.\n";
    } else {
        echo "The directory $dirname exists.\n";
    }

    //download json
    file_put_contents("./bus/$folder/$file", file_get_contents("$path"));
    //unzip
    $file_name = $out_file_name = "./bus/$folder/$file";
    $file=gzfile($file_name);  //get file into an array
    $out_file = fopen($out_file_name, 'w'); 
    $st=implode("",$file);  //turn array into string
    fwrite($out_file,$st);  //write the string in the out_file
    fclose($out_file);
}

?>