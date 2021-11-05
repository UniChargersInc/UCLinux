var mTb, mTv, _P = 1, gO, AJX, _myurl = "/", 
    myurl = AURL = "/w.ajx", 
    cPath = "r_files/common/", tPath = cPath + "icons/tree/", mPath = cPath + "icons/map/", a_sel;
var lgin = ["1", "????", "", 0]/* ["3602", "k123", "", 0]*/, t_hb = "",gRpt;
function gPrms(n) { return "req=" + n + "&cid=" + _el("pass_id_id").innerHTML + "&user=" + lgin[1] + "&rnd=" + g_rand() }
function LO() { window.location = "index.html" }
function onLoad()
{
    t_hb = Date.now();
}

function _onload() {
   var prms = gPrms(7);
    AJX.post(myurl, prms, function (txt) {
        var v = txt;
        if (v.indexOf("??", 0) != -1) LO();
        else {
            var x = v.split(","); // cid,user,pass 
            var n = x.length;
            if (n >= 3) {
                for (var i = 0; i < n; i++) {
                    if (x[i] != "")
                        lgin[i] = x[i];
                    else
                        LO();
                }
                window.onbeforeunload=function(){
				   unloadA();
				}
                // here we must dynamicaly load templates content
               // AJX.post(myurl, gPrms(10), function (v) {
                    
                   // var elm = _el("m_content");
                   // elm.innerHTML = v;
                    // setUp language here
                    /*var fn = "locale.js";
                    if (lgin[3] == 1)
                        fn = "es_" + fn;
                    loadJsCss("mkr_files/app/" + fn, "js");*/
                    _onready();
                    //fake it!!!
                    //lgin[0] = "3500";
                    //lgin[1] = "wilinx";
                    //lgin[2] = "****";
                    //onload();
                   // window.addEventListener('resize', _resize);
               // });
            } 
			else{ 
			  //LO();  //comment out for not to leave the page !!!
			}
        }

    });
}

function _onload_test() {
   var prms = gPrms(7);
    AJX.post(myurl, prms, function (txt) {
        var v = txt;
        if (v.indexOf("??", 0) != -1) LO();
        else {
            
            if (true) {
                //fake it!!!
                    lgin[0] = "1";
                    lgin[1] = "Ruks";
                    lgin[2] = "****";
                window.onbeforeunload=function(){
				   unloadA();
				}
                // here we must dynamicaly load templates content
                AJX.post(myurl, gPrms(10), function (v) {
                  
                    _onready();
                    //fake it!!!
                    //lgin[0] = "3500";
                    //lgin[1] = "wilinx";
                    //lgin[2] = "****";
                    //onload();
                   // window.addEventListener('resize', _resize);
                });
            }
        }

    });
}


function unloadA()
{
	/*AJX.post(myurl,gPrms(8) , function (d) {
        var v = d;
		
    });*/
}
function unLoad() {}
function _onready() {
   
    var tree = {
        id: "m_tree_id",
        view: "tree",
        type: "lineTree",
        select: true,
		drag:true,
        data: [
                { id: "1", value: "Assets" ,icon:tPath+"map/m119.png"},
               // { id: "2", value: "Mobile Workers" ,icon:tPath+"map/m133.png"},
               // { id: "3", value: "POIs",icon:tPath+"poit/poi.png" }
        ],

        on: {
            /*
            onDataRequest: function (id) {
                mkrwa.message("Getting children of " + id);
                this.parse(
                        AJX.get("data/data_dyn_json.php?parent=" + id).then(function (data) {
                            return data = data.json();
                        })
                );
                return false;
            }
            */
			onBeforeDrop:function(context){
				context.parent = context.target;    //drop as child
				context.index = -1; 
			}

        },
		template:tree_tmp_fn,
		//template:"{common.icon()} <img src='#icon#' style='float:left; margin:3px 4px 0px 1px;'> #value#",
        ready: function () {
           
        }
    };

 
    var appui = {
        id: "tb_tb1",
        type: "space", 
		cols: [
			{
			    width: 400, css: "bigHeader",
			    header: "Asset branches", headerHeight: 40,
			   // body: tree//countries_table
			   body:{ rows:[
			  		 {body:tree},
					 {
						view:"list", 
						id:"sts_did_lst",
						height:85 ,
						template:did_sts_tmp_fn, //"#name# - #val#",
						autoheight:true,
						data:[ {id:"1", name:"DeviceID:", val: ""},
								{id:"2", name:"Fan status:", val:""},
					 	]
					 }
					]
			   }
			},
			{ view: "resizer" },
			dashboard_charts
		]
    };
    var toolbar = {
        view: "toolbar",
		css: "highlighted_header header1",
        paddingY: 0,
        cols: [
          
            { view: "button",id:"tb_btn_1", type: "iconButton", icon: "dashboard", label: "Dashboard" ,click:tbf1},
			{ view: "button",id:"tb_btn_2",width:160, type: "iconButton", icon: "cog", label: "Settings" ,click:tbf2},
            { view: "button", type: "iconButton", icon: "bar-chart", label: "Reports",click:tbf3 },
			{ view: "button", type: "iconButton", icon: "sign-out", label: "Logout",click: LO },
            { gravity: 6 },
			{ view: "label", id: "dev_id_lbl" ,label:""},
			//{ view: "icon", icon: "search" ,popup:"search_pop"},
           // { view: "icon",id:"email_btn_id",width:60, icon: "envelope", /*badge:1 ,*/badge:1,popup:"email_pop"},
            { view: "icon",id:"conf_dev_id",width:60, icon: "cog" ,/*badge:12,*/click:tb_conf_fn},
             
        ]
    };
	/*
    var menu_data = [
		{
		    id: "1", value: "Fleet", icon: "automobile", submenu: [
                { id: "11", value: "Tracking", icon: "car" },
                { id: "12", value: "Configuration", icon: "edit" },
                { id: "13", value: "Geofences", icon: "globe" },
			    { id: "14", value: "Trip Replay", icon: "qrcode" },
		    ]
		},
        {
            id: "2", value: "Reports", icon: "bar-chart", submenu: [
                { id: "21", value: "Tracking", icon: "car" },
                { id: "22", value: "HOS", icon: "clock-o" },
                { id: "23", value: "DVIRs", icon: "qrcode" },
			   
            ]
        },
        { id: "3", value: "Work Orders", icon: "dashboard"},
        { id: "4", value: "Routing", icon: "random"},
		{ id: "5", value: "Help", icon: "question-circle"},
		{ id: "6", value: "Logout", icon: "sign-out", }
    ];
    var menu = {
        view: "menu",
        data: menu_data,
        css: "blue",
        on: {
            onMenuItemClick: function (id) {
                var x = parseInt(id);
                switch (x) {
                    case 3:
                        if(!gWO)
							gWO=new WORDERS(id);
                        break;
                   
                    case 4:
                        if(!gRtg)
						 gRtg=new ROUTE(id);
                        break;
                    case 5:
                        break;
                    case 6:
                        LO();
                        break;
                    case 12:
                        USERS(id);
                        break;
					case 22:
					if(!gRHos)
                        gRHos=new RHOS(id);
						break;
					case 23:
					if(!gRpt)
                        gRpt=new INS(id);
						break;
                }
               
            }
        },
    };*/
    var cellsD = [
           { id: "t1", header: "Dashboard", body: appui, width: 160 },
         
    ];
    mkrwa.ui({
        rows: [
            { type: "clean", cols: [/*menu,*/ toolbar] },
            { view: "tabview", id: "m_tabview", cells:/* mkrwa.copy(cells)*/cellsD },
            //appui
        ]
    });

    //TRACK("tb1");
   // mkrwa.ui(ui_order);

    //$$("dsc").bind($$('countries'));

    mTv=$$("m_tabview");
    mTb = mTv.getTabbar();
    mTb.attachEvent("onBeforeTabClose", function (id, e) {
       // alert(id);
	   var s = id.split("_");
	
	   switch(parseInt(s[1]))
	   {
		   case 3:
		   if(gRpt){
		   	 gRpt.dispose();
			 gRpt=null;
		   }
		   break;
		   case 4:
		   if(gRtg)
		   {
			   gRtg.dispose();
			   gRtg=null;
		   }
		   break;
		   case 22:
		   if(gRHos)
		   {
			   gRHos.dispose();
			   gRHos=null;
		   }
		   break;
	   }
	   
    });
	
	window.setTimeout(m_timer,5000);


	//init popups
	/*mkrwa.ui({
		view:"popup",
		id:"email_pop",
		head:"Submenu",
		width:300,
		body:{
			view:"list", 
			data:[ {id:"1", name:"Daniel Herby", location: "******"},
					{id:"2", name:"Sam Sanders", location:"========"},
					
			],
			datatype:"json",
			template:"#name# - #location#",
			autoheight:true,
			select:true
		}
	});*/
	/*
	mkrwa.ui({
		view:"popup",
		id:"msg_pop",
		head:"Submenu",
		width:500,height:600,
		body:{}//chat_body
	});
	var form1 = [
			{ view:"search", value:'', label:""},
			
		];
	
	mkrwa.ui({
		view:"popup",
		id:"search_pop",
		head:"Submenu",
		width:300,
		body:{ view:"form", scroll:false, width:300, elements: form1 },
	});
*/	
	var s1="Add new branch",s2="Delete branch",s3="Add device";
	 mkrwa.ui({
        view:"contextmenu",
	    id:"cmenu",
        data:[s1,{ $template:"Separator" },s2,{ $template:"Separator" },s3],
        on:{
            onItemClick:function(id){
                var context = this.getContext();
                var list = context.obj;
                var brId = context.id;
               // mkrwa.message("List item: <i>"+list.getItem(listId).title+"</i> <br/>Context menu item: <i>"+this.getItem(id).value+"</i>");
			   
			   if(id.indexOf(s1)!=-1)
			   {
				   show_add_branch();
			   }else if(id.indexOf(s2)!=-1)
			   {
				  mkrwa.confirm("Are you sure you want to delete this branch?", function(result){
					if(result)
					{
						delete_branch(brId);
					}
				 });
				   
			   }
			   else if(id.indexOf(s3)!=-1)
			   {
				   show_add_device(brId);
			   }
			   else if(id.indexOf("Info")!=-1)
			   {
				   mkrwa.alert({ok:"OK",type:"alert-warning",text:"Branch ID:"+brId});
			   }
            }
        }
    });
	var x_tree=$$("m_tree_id");
    $$("cmenu").attachTo(x_tree);
	
	x_tree.attachEvent("onBeforeDrop", function(context, ev){
		updateDevWithBranch(context);
		return true;
	});
	
	_init_done();
	
	
}//function _onready() {
function updateDevWithBranch(ctx)
{
	var brId=ctx.target;
	var did=ctx.source[0],s = brId.split("_");
	if(s.length>0)
	{
		var	v = gPrms(62) +"&did="+did+"&branchid=" + s[0];
		AJX.post(myurl, v, function (d) {	/*alert(d);*/});
	}
}
function delete_branch(id)
{
	var s = id.split("_"),bid=id;
	
	if(s.length>0)
	{
		if(!a_tree.isBranch(id))
		{
			var	v = gPrms(61) +"&branchid=" + s[0];
		
			AJX.post(myurl, v, function (d) {
				//alert(d);
				a_tree.remove(bid);
			});
		}
		else{
			mkrwa.alert({ok:"OK",type:"alert-warning",text:"You can not delete non empty branch"});
		}
	}
}
function show_add_branch()
{
	var ui_add_br_pu_bd = {
		height:300,
		width:400,
		rows:[
			{ view: "text", id: "d_br_n_id",  width: 300, labelWidth: 140,labelPosition:"top", value: "", label: "Branch Name "}
		]
	};
	//windows
		var add_branch_pu = {
			view: "window", modal: true, id: "add_br_win", 
						height:300,
						width:400,
						left:200, top:100,
						move:true,
						//resize: true,
						css: "bradius",
			//head: { view:"toolbar", cols:[ { view:"label", template:"Add new order"} ] },
			head: {view:"toolbar", cols:[
								{view:"label",id:"lbl_title_id", label: "Add new branch"  },
								{ view:"button",width:160, icon:"save",type:"iconButton",label:"Save and Exit",click:addBranch},
								{ view:"icon", icon:"times-circle",	click:"$$('add_br_win').close();"}
								] 
				  },
			body: ui_add_br_pu_bd 
		};
		
		mkrwa.ui(add_branch_pu).show();
}
function addBranch()
{
	var x=$$("d_br_n_id");
	var nn=x.getValue();
	var json={ID:-1,name:nn,icon:""};
	var	v = gPrms(60) +"&json=" + JSON.stringify(json);
	
	AJX.post(myurl, v, function (d) {
			if(d.indexOf("Error")!=-1)
			 //alert(d);
			 mkrwa.alert({ok:"OK",type:"alert-warning",text:"Please select a branch first",/*callback:function(){...}*/	});
			 else{
				 // add branch to the tree
				 var s = d.split(":");
				 if(s.length>1)
				 {
					 var bId=s[1],a_t,img;
					 a_t = bId + "_at";   // asset type
					img = tPath+"map/m112.png";// + at.Icon;
					a_tree.add({ id: a_t, value: nn ,icon:img}, 1, "1");
				 }
			 }
		});
	$$('add_br_win').close();
}

//--------------------add device-----------------
var branchId2Adddevice="";
function show_add_device(id)
{
	var s = id.split("_");
	branchId2Adddevice="";
	if(s.length>0)
	{
		branchId2Adddevice=s[0];
	}
	var ui_add_dev_pu_bd = {
		height:300,
		width:400,
		rows:[
			{ view: "text", id: "d_dev_id_id",  width: 300, labelWidth: 140,labelPosition:"top", value: "", label: "Device ID "},
			{ view: "text", id: "d_dev_n_id",  width: 300, labelWidth: 140,labelPosition:"top", value: "", label: "Device Name "}
		]
	};
	//windows
		var add_dev_pu = {
			view: "window", modal: true, id: "add_dev_win", 
						height:300,
						width:400,
						left:200, top:100,
						move:true,
						//resize: true,
						css: "bradius",
			//head: { view:"toolbar", cols:[ { view:"label", template:"Add new order"} ] },
			head: {view:"toolbar", cols:[
								{view:"label",id:"lbl_title_id", label: "Add device to this branch"  },
								{ view:"button",width:120, icon:"save",type:"iconButton",label:"Save",click:addDev2Branch},
								{ view:"icon", icon:"times-circle",	click:"$$('add_dev_win').close();"}
								] 
				  },
			body: ui_add_dev_pu_bd 
		};
		
		mkrwa.ui(add_dev_pu).show();
}
function addDev2Branch()
{
	var x=$$("d_dev_n_id");
	var nn=x.getValue(),id=-1;
	x=$$("d_dev_id_id");
	did=x.getValue();
	
	var	v = gPrms(63) + "&branchid=" + branchId2Adddevice+"&devid=" + did+"&desc=" + nn;
	
	AJX.post(myurl, v, function (d) {
			if(d.indexOf("Error")!=-1)
			 //alert(d);
			 mkrwa.alert({ok:"OK",type:"alert-warning",text:"We could not add this device",/*callback:function(){...}*/	});
			 else{
				 // add branch to the tree
				// var s = d.split(":");
				 //if(s.length>1)
				 {
					 var bId=/*s[1]*/branchId2Adddevice,a_t,img;
					 var a_t = bId + "_at";   // asset type
					var z =tPath+ "status/21.png"; // must be based on device type 21 - inactive // 22 - active
                    a_tree.add({id:did,value:nn,icon:z},1,a_t);//t.insertNewChild(a_t,a.Did + "_a", a.Desc, 0, z, z, z);
                    
				 }
			 }
		});
	$$('add_dev_win').close();
}
//----------------------------------------------------------
	
var a_tree,sel_d={did:"",fan:0};
function _init_done()
{
	 a_tree = $$("m_tree_id");
           
            a_tree.attachEvent("onItemClick", function (id) {
                //alert("Clicked.."+id);
				if(id.indexOf("_at")==-1)
				{
					$$("dev_id_lbl").setValue("DeviceID:"+id);
					sel_d.did = id;
					sel_d.fan = 0;               // -------------if it's online we should show status !!! 
					$$("sts_did_lst").refresh();
					setTimeout(processSel, 1000);
				}
            });
			a_tree.attachEvent("onAfterContextMenu", function (id) {
                var x=$$("cmenu");
				if(id.indexOf("_at")!=-1)
				x.show();
				else
				{
					x.hide();
					show_add_branch();
					/*mkrwa.confirm("Do you want to create a new branch?", function(result){
						if(result)
						{
							show_add_branch();
						}
					 });*/
					
				}
            });
			
			initTree(a_tree);
			
}
function processSel() {
	var aa=getAsset(sel_d.did);
	switchUpDB(aa);
}
function initTree(xx)
{
	 AJX.post(myurl/* + "LoginTest"*/, gPrms(1), function (d) {
        //alert(loader.xmlDoc.responseText);
        gO = JSON.parse(d);
        var o = gO;
		if (o != null && o.res) {
            lgin.cid = o.cid;
						
            createAssets(xx,o,true);
			
			xx.openAll();
        }
	 });
}

function createAssets(t,o,f) {
     var at,at_id,a_t,img,alen,j;   
      
    if (o.branches != null && o.assets != null) {
        for (j = 0; j < o.branches.length; j++) {
            at = o.branches[j];
            atid = at.BranchID;
            a_t = atid + "_at";   // asset type
            img = tPath+"map/m112.png";// + at.Icon;
            t.add({ id: a_t, value: at.Name ,icon:img}, j, "1");//t.insertNewChild(1, a_t, at.Desc, 0, img, img, img);
			alen=o.assets.length;
            for (var i = 0; i < alen; i++) {
                var a = o.assets[i];
                if (atid == a.BranchID) {
                    var z =tPath+ "status/21.png"; // must be based on device type 21 - inactive // 22 - active
                    //if (a.type == 1)
                    //    z =tPath+ "status/26.png";
                    t.add({id:a.DeviceID,value:a.Desc,icon:z},i,a_t);//t.insertNewChild(a_t,a.Did + "_a", a.Desc, 0, z, z, z);
                    
					a.done=1;
                }
            }
        }
		
		var flag=0;
		for (var i = 0; i < alen; i++)
		{
			var a=o.assets[i];
			if(isNDef(a.done))
			{
				flag=1;
				break;
			}
		}
		if(flag)
		{
			a_t = "65355" + "_at";   // asset type
            img = tPath+"map/m115.png" ;
            t.add({ id: a_t, value: "Unassigned" ,icon:img}, j, "1");
			
			 for (var i = 0; i < alen; i++) {
                var a = o.assets[i];
                if(isNDef(a.done)) {//if (atid == a.BranchID) {
                    var z =tPath+ "status/21.png"; // must be based on device type
                    //if (a.type == 1)
                    //    z =tPath+ "status/26.png";
                    t.add({id:a.DeviceID,value:a.Desc,icon:z},i,a_t);//t.insertNewChild(a_t,a.Did + "_a", a.Desc, 0, z, z, z);
                    
                }
            }
			
		}
    }
}

mkrwa.ready(function () {
    AJX=mkrwa.ajax();
	if(_P)_onload();else _onload_test();
 
});

//===========================m timer=========================================================
function m_timer_cb(tt) {
        
		if (tt.indexOf("e_x_c") != -1)LO();
        var O = JSON.parse(tt);
        if (O != null) {
			t_hb=new Date().toISOString(); 
		     //real time messages from mobile or mdt
            if(O.length>0)
            for (i = 0; i < O.length; i++) {
              
                process_tree(O[i]);
            }
           
			
        }//if (O != null) 
		else
		   LO();
}
function m_timer() {
	var par=gPrms(2)+"&ts=" + t_hb;;
    try{
	AJX.post(myurl,par,m_timer_cb);
	}catch(e){LO()}
    window.setTimeout(m_timer, 5000);

    //updateNEmails(nemails++);
}
function tree_tmp_fn(obj,common)
{
	/*
	if(obj.$level == 1)
                return "<span class='mkrwa_icon fa-folder-open'></span>";
            if (obj.$level == 2)
                return "<span class='mkrwa_icon fa-video-camera'></span>";
   */
   //<img src='#icon#' style='float:left; margin:3px 4px 0px 1px;'>
   var ci=common.icon(obj,common);//,cf=/*common.folder(obj,common)*/"<img src='#icon#' style='float:left; margin:3px 4px 0px 1px;'>";
   var cf="<img src='"+obj.icon+"' style='float:left; margin:3px 4px 0px 1px;'>";
   if (obj.$level>2) {
	   var h="<span><span>"+obj.value+"</span></span>"
       return ci+cf+h;//"<i>"+obj.value+"</i>";
    } else {
		var h="<span><span class='mkr_tree_cust'>"+obj.value+"</span></span>";
       return ci+cf+h;
    }

}

function process_tree(x)
{
	  var f=false;
	  a_tree.data.each(
	
		 function(o){
			 if(!a_tree.isBranch(o.id))
			 {
				 var p=o.$parent;
				 if(typeof p=="string" && p.indexOf("_at")!=-1)
				 {
					 //o.value=o.value+count;
					 if(o.id==x.devID)
					 {
						 var a=getAsset(o.id);
						 if(a)
						 {
							 o.icon=tPath+"status/22.png"; 
					 		 f=true;
							 updateDashBoard(a,x);
									//if(pin)pin.a=as;
						 }
						
					 }
				 }
			 }
		} 
	 );
	 if(f)
	 a_tree.refresh();
}
function getAsset(id) {
    var res = null;
    if (gO != null) {
        var aa = gO.assets;
        if (aa != null) {
            var l = aa.length;
            for (var i = 0; i < l; i++) {
                var a = aa[i];
                if (a.DeviceID == id) {
                    res = a;
                    break;
                }
            }
        }
    }
    return res;
}


function updateDashBoard(a, o)
{
	//a - asset , o - rtt data from device 
	var did=o.devID,data=o.data;
	if(isNDef(a.dataArr))
	 	a.dataArr=[];
	
	if(a.dataArr.length>20)
	{
		removeFirstArrEl(a.dataArr);
	}
	a.dataArr.push(data);
	//for test only
	/*var aa=getAsset(did);
	if(isNDef(aa.dataArr))
	{
	}*/
	if(sel_d.did==did)
	{
		updateChart(data,a);
		sel_d.fan = data.Ozone & 0x8000 ? 1 : 0;  // fan flag
		$$("sts_did_lst").refresh();
	}
}
//============================================================
function did_sts_tmp_fn(obj)
{
	var d=isEmpty(sel_d.did)?"":sel_d.did;
	if(obj.id=="1")
	{
		obj.val=d;
		
	}
	if(obj.id=="2")
	{
		if(d=="")obj.val="";else
		obj.val=sel_d.fan?"ON":"OFF";
		
	}
	return obj.name+" "+obj.val;
}
//=====================toolbar functions call=================
function tbf1()
{
	
}
function tbf2()
{
	//if(!gRpt)
      // gRpt=new INS("tb_2");
	  showConf(gO.conf);
}
function tbf3()
{
	if(!gRpt)
       gRpt=new RPT("3");
}
/*
function updateNEmails(n) {
    var x = $$("email_btn_id");
    x.define("badge", n);
    x.refresh();
}
*/
function tb_conf_fn() {
	if(!isEmpty(sel_d.did))
	{
    	var a=getAsset(sel_d.did);
		if(!isNDef(a)) 
		    showDevConf(a);
	}else{
		mkrwa.alert({
			//title:"Custom title",
			ok:"OK",
			type:"alert-warning",
			text:"Please select a device first",
			//callback:function(){...}
		});
	}
}


//=====================================================
		