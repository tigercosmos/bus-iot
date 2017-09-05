<?php  
//download and decode
/*Route*/
$folder="Route";
$file=date('ymdhi');
$path="GetRoute";
download($folder,$file,$path);
$route_content = file_get_contents("./bus/$folder/$file");
$route = json_decode($route_content,true);
/*Stop*/
$folder="Stop";
$file=date('ymdhi');
$path="GetStop";
download($folder,$file,$path);
$stop_content = file_get_contents("./bus/$folder/$file");
$stop = json_decode($stop_content,true);


//deal with data
/*station_path*/
$stop_path = [];
for($i=0;$i<count($stop["BusInfo"]);$i++){
    $exist=false;
    $info=array();
    //check if the stop has appeared. if yes,break
    for($k=$i-1;$k>=0;$k--){
        if($stop["BusInfo"][$k]["nameZh"]==$stop["BusInfo"][$i]["nameZh"]){
            $exist=true;
            break;            
        }
    }
    if(!$exist){
        for($k=$i;$k<count($stop["BusInfo"]);$k++){
            if($stop["BusInfo"][$k]["nameZh"]==$stop["BusInfo"][$i]["nameZh"]){
                for($t=0;$t<count($route["BusInfo"]);$t++){
                    if($route["BusInfo"][$t]["Id"]==$stop["BusInfo"][$k]["routeId"]){
                        $name=$route["BusInfo"][$t]["pathAttributeName"];
                        $id=$route["BusInfo"][$t]["pathAttributeId"];
                        $info[]=array("name"=>$name,"id"=>$id);
                        break;
                    }
                }

            }
        }
        $stop_path[$stop["BusInfo"][$i]["nameZh"]]=array("Info"=>$info);
    }
}
$result=array("station_path"=>$stop_path);
$data=json_encode($result);
//save it locally
//check dir
$dirname = "./bus/station_path";
if (!file_exists($dirname)) {
    mkdir($dirname, 0777, true);
    echo "The directory $dirname was successfully created.\n";
} 
else {
    echo "The directory $dirname exists.\n";
}
$myfile = fopen("./bus/station_path/data.json", "w") or die("Unable to open file!");
fwrite($myfile, $data);
fclose($myfile);
?>
