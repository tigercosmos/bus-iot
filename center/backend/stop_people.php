<?php
//get info
$station=$_GET["station"];
$path=$_GET["path"];

//get data for checking whether the info had repeated or not
$folder="monitor_getinfo";
$file=date('ymdhi');
download($folder,$file,$path);
$data_content = file_get_contents("./bus/$folder/$file");
$data = json_decode($data_content,true);

$people=0;
for($i=0;$i<count($data);$i++){
    if($data[$i]["id"]==$station){
        $people=$data[$i]["station_people"];
        $people++;
        break;
    }
}
$base_path="https://bus-all.firebaseio.com/monitor_bus_info/$path/stations/$i.json";
$data=json_encode(array("station_people"=>$people));
upload("PATCH",$data,$base_path);
$path1="https://bus-all.firebaseio.com/people/$path/$station.json";
upload("PATCH",$data,$path1);


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
    $temp = curl_exec($ch);
    //如果要在頁面上看到最後的值 因為之前設定過CURLOPT_RETURNTRANSFER 所以需要將執行後的string 印出echo才有
    echo $temp;
    //close curl
    curl_close($ch);

}


function download($folder,$file,$path){
//check dir
$dirname = "./bus/$folder";
if (!file_exists($dirname)) {
    mkdir($dirname, 0777);
    echo "The directory $dirname was successfully created.\n";
} else {
    echo "The directory $dirname exists.\n";
}

//download gz
file_put_contents("./bus/$folder/$file", file_get_contents("https://bus-all.firebaseio.com/monitor_bus_info/$path/stations.json"));
//unzip
$file_name = "./bus/$folder/$file";
$out_file_name = $file_name;
$file=gzfile($file_name);  //get file into an array
$out_file = fopen($out_file_name, 'w'); 
$st=implode("",$file);  //turn array into string
fwrite($out_file,$st);  //write the string in the out_file
fclose($out_file);
}