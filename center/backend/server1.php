<?php  
//download and decode
/*Bus Event*/
$folder="Bus_Event";
$file=date('ymdhi');
$path="GetBusEvent";
download($folder,$file,$path);
$event_content = file_get_contents("./bus/$folder/$file");
$event = json_decode($event_content,true);
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
/*Estimate Time*/
$folder="Estimate_Time";
$file=date('ymdhi');
$path="GetEstimateTime";
download($folder,$file,$path);
$time_content = file_get_contents("./bus/$folder/$file");
$time = json_decode($time_content,true);
/*rfid*/
$folder="rfid";
$file=date('ymdhi');
$dirname = "./bus/$folder";
if (!file_exists($dirname)) {
    mkdir($dirname, 0777);
    echo "The directory $dirname was successfully created.\n";
} else {
    echo "The directory $dirname exists.\n";
}
//download json
file_put_contents("./bus/$folder/$file", file_get_contents("https://bus-all.firebaseio.com/bus_rfid.json"));
$rfid_content = file_get_contents("./bus/$folder/$file");
$rfid = json_decode($rfid_content,true);
/*people*/
$folder="people";
$file=date('ymdhi');
$dirname = "./bus/$folder";
if (!file_exists($dirname)) {
    mkdir($dirname, 0777);
    echo "The directory $dirname was successfully created.\n";
} else {
    echo "The directory $dirname exists.\n";
}
//download json
file_put_contents("./bus/$folder/$file", file_get_contents("https://bus-all.firebaseio.com/people.json"));
$people_content = file_get_contents("./bus/$folder/$file");
$people = json_decode($people_content,true);


/*deal with data*/
/*path*/
$base_path="https://bus-all.firebaseio.com/.json";
for($i=0;$i<count($route["BusInfo"]);$i++){
    $info[]=array("id"=>$route["BusInfo"][$i]["pathAttributeId"],"name"=>$route["BusInfo"][$i]["pathAttributeName"]);
}
$result=array("path"=>$info);
$data=json_encode($result);
patch($base_path,$data);

/*monitor bus info*/
$base_path="https://bus-all.firebaseio.com/monitor_bus_info.json";
for($i=0;$i<count($route["BusInfo"]);$i++){
    //bus
    $info=array();
    $number=0;  //calculate the number of buses on the certain path
    for($t=0;$t<count($event["BusInfo"]);$t++){
        //number of people on the certain car
        $car_id=$event["BusInfo"][$t]["CarID"];
        $rfid_info=array();
        $bus_people=count($rfid[$car_id])-1;
        if($event["BusInfo"][$t]["RouteID"]==$route["BusInfo"][$i]["pathAttributeId"]){ 
            /*Not on duty*/
            if($event["BusInfo"][$t]["DutyStatus"]==2){
                $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>"Not on duty","bus_people"=>$bus_people);
            }
            /*On duty*/
            elseif($event["BusInfo"][$t]["DutyStatus"]==1||$event["BusInfo"][$t]["DutyStatus"]==0){
                $order=stop_name_order($event["BusInfo"][$t]["RouteID"],$stop,$route); 
                /*去程*/
                if($event["BusInfo"][$t]["GoBack"]==0){
                    $stop_no;
                    //looking for order of the certain stop on the path
                    for($j=0;$j<count($stop["BusInfo"]);$j++){
                        if($stop["BusInfo"][$j]["Id"]==$event["BusInfo"][$t]["StopID"]){
                            $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            /*找最小的seqNo*/
                            if($stop["BusInfo"][$j]["seqNo"]<$stop_no){
                                $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            }
                        }
                    }
                    //check the bus is leaving for the certain stop or leaving to
                    /*進站*/
                    if($event["BusInfo"][$t]["CarOnStop"]==0){
                        if($stop_no==0){
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>"Not on duty","bus_people"=>$bus_people);
                        }
                        else{
                            $number++;
                            $no=$stop_no;
                            $no1=$no-1;
                            $position=array($order[$no1],$order[$no]);
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>$position,"bus_people"=>$bus_people);
                        }
                    }
                    /*離站*/
                    elseif($event["BusInfo"][$t]["CarOnStop"]==1){
                        if($stop_no==count($order)-1){
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>"Not on duty","bus_people"=>$bus_people);
                        }
                        else{
                            $number++;
                            $no=$stop_no;
                            $no1=$no+1;
                            $position=array($order[$no],$order[$no1]);
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>$position,"bus_people"=>$bus_people);
                        }
                    }
                }
                /*回程*/
                elseif($event["BusInfo"][$t]["GoBack"]==1){
                    //looking for order of the certain stop on the path
                    for($j=0;$j<count($stop["BusInfo"]);$j++){
                        if($stop["BusInfo"][$j]["Id"]==$event["BusInfo"][$t]["StopID"]){
                            $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            /*找最大的seqNo*/
                            if($stop["BusInfo"][$j]["seqNo"]>$stop_no){
                                $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            }
                        }
                    }
                    //check the bus is leaving for the certain stop or leaving to
                    /*進站*/
                    if($event["BusInfo"][$t]["CarOnStop"]==0){
                        if($stop_no==0){
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>"Not on duty","bus_people"=>$bus_people);
                        }
                        else{
                            $number++;
                            $no=$stop_no;
                            $no1=$no-1;
                            $position=array($order[$no1],$order[$no]);
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>$position,"bus_people"=>$bus_people);
                        } 
                    }
                    /*離站*/
                    elseif($event["BusInfo"][$t]["CarOnStop"]==1){
                        if($stop_no==count($order)-1){
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>"Not on duty","bus_people"=>$bus_people);
                        }
                        else{
                            $number++;
                            $no=$stop_no;
                            $no1=$no+1;
                            $position=array($order[$no],$order[$no1]);
                            $info[]=array("id"=>$event["BusInfo"][$t]["BusID"],"no"=>$event["BusInfo"][$t]["CarID"],"position"=>$position,"bus_people"=>$bus_people);
                        }      
                    }
                } 
            }
        }
    }
    //stations
    //looking for the certain route id from $stop
    $stop_info=array();
    $order=array();
    for($t=0;$t<count($stop["BusInfo"]);$t++){
        if($stop["BusInfo"][$t]["routeId"]==$route["BusInfo"][$i]["Id"]){
            /*order*/
            $stop_name=$stop["BusInfo"][$t]["nameZh"];
            $stop_order=$stop["BusInfo"][$t]["seqNo"];
            $order[$stop_order]=$stop_name;
            /*stations*/
            $id=$stop["BusInfo"][$t]["Id"];
            $name=$stop["BusInfo"][$t]["nameZh"];
            //先從$people抓目前的人數再上傳
            $people_num=$people[$route["BusInfo"][$i]["pathAttributeId"]][$id]["station_people"];
            $stop_info[$id]=array("name"=>$name,"station_people"=>$people_num);
        }
    }
    ksort($order);
    $stop_number=count($order);
    //in order to confirm no blank index exists,renumber the stops
    $no=array();
    for($k=0;$k<$stop_number;$k++){
        array_push($no,$k);
    }
    $reorder=array_combine($no,$order);
    
    /*all*/
    $bus[$route["BusInfo"][$i]["pathAttributeId"]]=array("bus_number"=>$number,"buses"=>$info,"stations"=>$stop_info,"order"=>$reorder,"station_number"=>$stop_number);
}
$data=json_encode($bus);
patch($base_path,$data);

/*next*/
$next_stop;
$next_people;
$stop_no;
for($i=0;$i<count($route["BusInfo"]);$i++){
    $next=array();
    for($t=0;$t<count($event["BusInfo"]);$t++){
        //找該路線下的行駛中的車次
        if($event["BusInfo"][$t]["RouteID"]==$route["BusInfo"][$i]["Id"]){
            /*On duty*/
            if($event["BusInfo"][$t]["DutyStatus"]==1||$event["BusInfo"][$t]["DutyStatus"]==0){
                //order the stops on the certain path
                $order=stop_id_order($event["BusInfo"][$t]["RouteID"],$stop,$route); 
                /*去程*/
                if($event["BusInfo"][$t]["GoBack"]==0){
                    //looking for order of the certain stop on the path
                    for($j=0;$j<count($stop["BusInfo"]);$j++){
                        if($stop["BusInfo"][$j]["Id"]==$event["BusInfo"][$t]["StopID"]){
                            $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            /*找最小的seqNo*/
                            if($stop["BusInfo"][$j]["seqNo"]<$stop_no){
                                $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            }
                        }
                    }
                    //check the bus is leaving for the certain stop or leaving to
                    /*進站*/
                    if($event["BusInfo"][$t]["CarOnStop"]==0){
                        $next_stop=$order[$stop_no];
                        if($people[$route["BusInfo"][$i]["pathAttributeId"]][$next_stop]["station_people"]!=0){
                            $next_people=true;
                            $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                        }   
                        else{
                            $next_people=false;
                            $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                        }     
                    }
                    /*離站*/
                    elseif($event["BusInfo"][$t]["CarOnStop"]==1){
                        //末站
                        if($stop_no==count($order)-1){
                            $next_people=false;
                            $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                        }
                        else{
                            $next_stop=$order[$stop_no+1];
                                if($people[$route["BusInfo"][$i]["pathAttributeId"]][$next_stop]["station_people"]!=0){
                                    $next_people=true;
                                    $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                                }   
                                else{
                                    $next_people=false;
                                    $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                                } 
                        }
                    }
                }
                /*回程*/
                elseif($event["BusInfo"][$t]["GoBack"]==1){
                    //looking for order of the certain stop on the path
                    for($j=0;$j<count($stop["BusInfo"]);$j++){
                        if($stop["BusInfo"][$j]["Id"]==$event["BusInfo"][$t]["StopID"]){
                            $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            /*找最大的seqNo*/
                            if($stop["BusInfo"][$j]["seqNo"]>$stop_no){
                                $stop_no=$stop["BusInfo"][$j]["seqNo"];
                            }
                        }
                    }
                    //check the bus is leaving for the certain stop or leaving to
                    /*進站*/
                    if($event["BusInfo"][$t]["CarOnStop"]==0){
                        $next_stop=$order[$stop_no];
                                if($people[$route["BusInfo"][$i]["pathAttributeId"]][$next_stop]["station_people"]!=0){
                                    $next_people=true;
                                    $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                                }   
                                else{
                                    $next_people=false;
                                    $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                                } 
                    }
                    /*離站*/
                    elseif($event["BusInfo"][$t]["CarOnStop"]==1){
                        //末站
                        if($stop_no==count($order)-1){
                            $next_people=false;
                            $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                        }
                        else{
                            $next_stop=$order[$stop_no+1];
                            if($people[$route["BusInfo"][$i]["pathAttributeId"]][$next_stop]["station_people"]!=0){
                                    $next_people=true;
                                    $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                            }
                            else{
                                $next_people=false;
                                $next[$event["BusInfo"][$t]["CarID"]]=array("next"=>$next_people);
                            }   
                        }      
                    }
                } 
            }
        }
    }
    $info[$route["BusInfo"][$i]["pathAttributeId"]]=$next;
}
$data=json_encode(array("next"=>$info));
$path="https://bus-all.firebaseio.com/.json";
patch($path,$data);

/*stop bus info*/
$estimate_time;
//一條路線一條路線找資料
for($i=0;$i<count($route["BusInfo"]);$i++){
    $next_bus=array();
    //一個站一個站找資料
    for($t=0;$t<count($stop["BusInfo"]);$t++){
        $no;
        $car_order=array();
        $car_id=array();
        if($stop["BusInfo"][$t]["routeId"]==$route["BusInfo"][$i]["Id"]){
            //紀錄該路線上的站
            $order=stop_id_order($route["BusInfo"][$i]["pathAttributeId"],$stop,$route);
            //estimate time
            for($k=0;$k<count($time["BusInfo"]);$k++){
                if($time["BusInfo"][$k]["StopID"]==$stop["BusInfo"][$t]["Id"]){
                    $estimate_time=$time["BusInfo"][$k]["EstimateTime"];
                    break;
                }
            }
            //looking for the next bus id
            for($x=0;$x<count($event["BusInfo"]);$x++){
                //looking for all the buses on duty on the certain path
                if($event["BusInfo"][$x]["RouteID"]==$route["BusInfo"][$i]["Id"]&&$event["BusInfo"][$x]["DutyStatus"]!=2){
                    for($car_position=0;$car_position<count($order);$car_position++){
                        if($order[$car_position]==$event["BusInfo"][$x]["StopID"]){
                            //recording their position and the bus id
                            array_push($car_order,$car_position);
                            array_push($car_id,$event["BusInfo"][$x]["CarID"]);
                            break;
                        }
                    }
                }
            }
            //if there is no infomation being recorded, which means there is no bus now
            if($car_id==null){
                $next_bus="There is no bus on the path";
                break;
            }
            else{
                //$no is the order of the certain stop
                for($n=0;$n<count($order);$n++){
                    if($order[$n]==$stop["BusInfo"][$t]["Id"]){
                        $no=$n;
                        break;
                    }
                }
                //looking for the minimum delta of order to determine which is the next bus
                $delta=1000;
                $bus_no=0;
                for($z=0;$z<count($car_order);$z++){
                    if(abs($car_order[$z]-$no)<$delta){
                        $delta=abs($car_order[$z]-$no);
                        $bus_no=$z;
                    }
                }
                //looking for the next bus info
                $bus_id=$car_id[$bus_no];
                $type="";
                for($a=0;$a<count($event["BusInfo"]);$a++){
                    if($event["BusInfo"][$a]["CarID"]==$bus_id)
                    {
                        if($event["BusInfo"][$a]["CarType"]==0){$type="一般";}
                        elseif($event["BusInfo"][$a]["CarType"]==1){$type="低底盤";}
                        elseif($event["BusInfo"][$a]["CarType"]==2){$type="大復康巴士";}
                        elseif($event["BusInfo"][$a]["CarType"]==3){$type="圓仔公車";}
                        break;
                    }
                }
                //rfid
                $car_people=count($rfid[$bus_id])-1;
                //all
                $next_bus[$stop["BusInfo"][$t]["Id"]]=array("estimate_time"=>$estimate_time,"next_bus_type"=>$type,"next_bus_people"=>$car_people);
            }
            
        }
    }
    $info[$route["BusInfo"][$i]["pathAttributeId"]]=$next_bus;
}
$data=json_encode($info);
$path="https://bus-all.firebaseio.com/stop_bus_info.json";
patch($path,$data);


function patch($base_path,$data){
    //init curl
    $ch = curl_init();
    //set curl options 設定你要傳送參數的目的地檔案
    curl_setopt($ch, CURLOPT_URL, "$base_path");
    curl_setopt($ch, CURLOPT_HEADER, false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
    //開啓patch選項
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_setopt($ch, CURLOPT_POSTFIELDS,$data);
    //execute curl
    $temp = curl_exec($ch);
    //如果要在頁面上看到最後的值 因為之前設定過CURLOPT_RETURNTRANSFER 所以需要將執行後的string 印出echo才有
    echo $temp;
    //close curl
    curl_close($ch);

}

function stop_name_order($route_id,$stop,$route){
    for($i=0;$i<count($route["BusInfo"]);$i++){
        if($route["BusInfo"][$i]["pathAttributeId"]==$route_id){
           for($t=0;$t<count($stop["BusInfo"]);$t++){
               if($stop["BusInfo"][$t]["routeId"]==$route["BusInfo"][$i]["Id"]){
                   $stop_name=$stop["BusInfo"][$t]["nameZh"];
                   $stop_order=$stop["BusInfo"][$t]["seqNo"];
                   $order[$stop_order]=$stop_name;
               }
           }
        }
    }
    ksort($order);
    $no=array();
    for($t=0;$t<count($order);$t++){
        array_push($no,$t);
    }
    $reorder=array_combine($no,$order);
    return $reorder;
}

function stop_id_order($route_id,$stop,$route){
    for($i=0;$i<count($route["BusInfo"]);$i++){
        if($route["BusInfo"][$i]["pathAttributeId"]==$route_id){
           for($t=0;$t<count($stop["BusInfo"]);$t++){
               if($stop["BusInfo"][$t]["routeId"]==$route["BusInfo"][$i]["Id"]){
                   $stop_name=$stop["BusInfo"][$t]["Id"];
                   $stop_order=$stop["BusInfo"][$t]["seqNo"];
                   $order[$stop_order]=$stop_name;
               }
           }
        }
    }
    ksort($order);
    $no=array();
    for($t=0;$t<count($order);$t++){
        array_push($no,$t);
    }
    $reorder=array_combine($no,$order);
    return $reorder;
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
file_put_contents("./bus/$folder/$file", file_get_contents("https://tcgbusfs.blob.core.windows.net/blobbus/$path.gz"));
//unzip
$file_name = "./bus/$folder/$file";
$out_file_name = $file_name;
$file=gzfile($file_name);  //get file into an array
$out_file = fopen($out_file_name, 'w'); 
$st=implode("",$file);  //turn array into string
fwrite($out_file,$st);  //write the string in the out_file
fclose($out_file);
}
?>