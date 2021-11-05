//function EL(d) { return document.getElementById(d) }
//function moveEnd() {
//    var bounds = gMap.getBounds();
//    var pos1 = gMap.project(bounds._ne);
//    var pos2 = gMap.project(bounds._sw);

//    gMap.getCanvas().style.cursor = 'pointer';
//}
function deg2rad(d) {
    return (d * Math.PI / 180.0);
}
function calculatePointTile( lon,  lat,  z) {
    var n = Math.pow(2, z);
    var xtile = n * ((lon + 180.0) / 360.0);
    var ytile = n * (1 - (Math.log(Math.tan(deg2rad(lat)) + (1.0 / Math.cos(deg2rad(lat)))) / Math.PI)) / 2;
    return { z: z, x: parseInt(xtile,10), y: parseInt(ytile,10) };
}


//function openNav() {

//    var el = EL("panel_id"), elb = EL("btn_arr_id"), t = elb.innerText;
//    if (t == ">") {
//        el.style.left = "0px";
//        elb.style.left = "0px";
//        t = "<";

//    } else {
//        el.style.left = "-300px";
//        elb.style.left = "-300px";
//        t = ">";
//    }
//    elb.innerText = t;
//}


//----------------------------------------------------------
function MkrGMap(name,_cb,flag) {
    this.name = name;
    var ths = this, _map, cb = _cb, dirctrl,prZ=0,prTA;
   
    this.$init = function (config) {
        this.$view.innerHTML = "<div class='mkrwa_map_content' style='width:100%;height:100%'></div>";
        ths._mapId = this.$view.firstChild;

        _map = null;
        this.$ready.push(this.render);
    }
    this.render = function () {
        this._initMap();

    }
    this._initMap = function (define) {
        //var c = this.config;
       // var stl = flag ? my_style_bright_v8 : my_style_bright_v8_sat;
       _map = new mapboxgl.Map({
            container: ths._mapId,//'map',
           
           // style: stl,
            zoom: 14,
            center: [-79.5, 43.8499]// center: [-122.4536, 37.796]
        });
        //_map.debug = true; // to show tiles boundaries
        _map.addControl(new mapboxgl.NavigationControl());
        //if (dirs) {
        //    var _dir = new MapboxDirections({
        //        accessToken: "", //'pk.eyJ1IjoiZGFucGF0IiwiYSI6ImNpcHIwOTBrMTA2am1meG5vdXE4eDN1aDkifQ.Ksnrtuh0LEJuFSVE-SlhxQ'
        //        unit: "metric"
        //    });
        //    dirctrl = _map.addControlM(_dir, 'top-left');
        //   // this.addDirs(1);
        //}
       
        ths.switchLayer(flag);
        //add scale panel
        var c = document.createElement("canvas");        // Create a <button> element

        c.style.bottom = "10px";
        c.style.right = "10px";
        c.width = 400;
        c.height = 10;
        c.style.position = "fixed";
        ths._mapId.appendChild(c);
        ths.scaleC = c;

        window.setTimeout(function () {
            if (cb) cb();// gmap_init_done();
            //_map.on('zoomend', function (e) {
            //    console.log('A zoomend event has occurred at ' + e.lngLat);

            //});
            _map.on('moveend', function (e) {
               // console.log('A MoveEnd event has occurred at ' + e.lngLat);
                var map = e.target,z,z1;
                z = map.getZoom(), z1 = Math.round(z);
                ths.showZoom(z);
                if (prZ != z1) {
                    prZ = z1;
                }
               // var tA = ths.moveEnd(z1);
               // prTA = tA;
            });

        }, 300);   // make a delay just to make sure that the map style gets loaded
        
    }
    this.moveEnd = function(z) {
        var bounds = _map.getBounds();
        // the followng two line shows screen coordinates
        var pos1 = /*_map.project*/(bounds._ne);
        var pos0 = /*_map.project*/(bounds._sw);

        
        //var t1 = calculatePointTile(-79.501930, 43.859999, 14);
       
        var t0 = calculatePointTile(pos0.lng, pos0.lat, z);
        var t1 = calculatePointTile(pos1.lng, pos1.lat, z);
        //uint64_t tIndex = (tilex << 16) + tiley;
       // _map.getCanvas().style.cursor = 'pointer';
        var tA = [];
        for (var x = t0.x; x <= t1.x; x++) {
            for (var y = t0.y; y >= t1.y; y--) {
                var tInd = (x << 16) + y;
                tA.push(tInd);
            }
        }
        return tA;
    }
    this.$setSize = function () {
        mkrwa.ui.view.prototype.$setSize.apply(this, arguments);
        if (_map) {
            _map.resize();
            ths.showZoom(14);
        }
    }
    this.attachDirs = function (el) {
        if (dirctrl)
            el.appendChild(dirctrl.ctrlEl);
    }
    this.openNav = function (/*e*/f) {
        var el = dirctrl.ctrlEl; //, elb = toggleBtn, t = elb.innerText;
        //if (t == ">") {
        //    el.style.left = "0px";
        //    elb.style.left = "0px";
        //    t = "<";

        //} else {
        //    el.style.left = "-300px";
        //    elb.style.left = "-300px";
        //    t = ">";
        //}
        //elb.innerText = t;
        if (f) {
            el.style.left = "0px";
            

        } else {
            el.style.left = "-300px";
            
        }
        
    }
    this.addDirs = function (f) {
        var el = dirctrl.ctrlEl;
        if (f) {
            
            el.style.display = "block";

            //svk addition
            //el.style.backgroundColor = "red";
            //el.style.border = "thick solid #0000FF";
            //el.style.margin = "2px 2px 2px 2px";

            el.style.width = "300px";
           // el.style.height = "400px";
            el.style.float = "left";
            el.style.position = "relative";
            el.style.left = "0px";
            el.style.transition = "0.5s";


           // var c = document.createElement("a");        // Create a <button> element
           // c.style.padding = "5px";
           // c.style.width = "10px";
           // c.style.height = "20px";
           // c.style.float = "left";
           // c.style.left = "0px";
           // c.style.font = "400 14px Arial";
           // c.style.fontWeight = "bold";
           // c.style.transition = "0.5s";
           // c.style.position = "relative";
           // c.style.backgroundColor = "#d9dada";
            
           //// dirctrl.Container.style.backgroundColor = "red";

           // //c.className = "mapboxgl-ctrl-top-left";

           //// el.appendChild(c);
           // //c.setAttribute('href', 'javascript:openNav()');
           // c.href = "#";
           // c.style.cursor = "pointer"; 
           // window.addEventListener("click", ths.openNav, false);
           //// dirctrl.Container.addEventListener("click", ths.openNav, false);  //==================================
           //// c.onclick = openNav;

           // c.innerText = ">";
           // c.style.zIndex = "1000"; 
           // toggleBtn = c;

           // dirctrl.Container.appendChild(c);

        } else {
            //dirctrl.style.display = "none";
            el.style.display = "none";
        }
    }
   
    this.map=function(){ return _map; }
    this.id = function () { return ths._mapId }
    this.scaleP = function () { return ths.scaleC }
    this.GetZoom = function () { return _map.getZoom() }
    this.setPos = function (lat, lon, zoom) {
        if (_map) {
            _map.setCenter([lon, lat]);
        }
    }
   
    this.setMarkerIcon = function (m, icPath)
    {
        if (m) {
            m._element.style.backgroundImage = "url(" + icPath + ")";
        }
    }
    this.add_marker = function (a,ic)
    {
        // create a HTML element for each feature
        var el = document.createElement('div');
        //el.className = 'marker';
        el.style.width = "24px";
        el.style.height = "24px";
        el.style.backgroundImage = "url("+ic+")";
        el.style.cursor = "pointer";
        el.style.borderRadius = "50%";

        // make a marker for each feature and add to the map
        var m = new mapboxgl.Marker(el)
            .setLngLat([a.lon, a.lat])  //[-79.414, 43.776]
            //.setPopup(new mapboxgl.Popup({ offset: 25 }) 
            //    .setHTML('<h3>' + 'Market Title' + '</h3><p>' + 'Market Desc' + '</p>'))
            .addTo(_map);
        return m;
    }
    this.addAMarker = function (a) {

        var m = this.add_marker(a, mIC+'009_h.png');
        m.asset = a;
        var popup = new mapboxgl.Popup({ offset: 12 });
        var h = "";//"<b style='color:blue'>" + a.Desc + "</b><b><br>DeviceID: " + a.Did + "<br>" + toDateStr(a.TS, 1) + "</b>";
        popup.setHTML(h);
        popup.addToCb = function (m) {
            var aa = m.asset;
            var h = "<b style='color:blue'>" + aa.Desc + "</b><b><br>DeviceID: " + aa.Did + "<br>" + toResDateStr(aa.TS, aa.Did) + "</b>";
            m._popup.setHTML(h);
        };
        m.setPopup(popup);
        return m;
    }
    this.addSMarker = function (a) {

        var m = this.add_marker(a, mIC + '009_h.png');
        m.asset = a;
        var popup = new mapboxgl.Popup({ offset: 12 });
        var h = "<b style='color:blue'>" + a.Desc + "<br>" + toDateStr(a.TS, 1) + "</b>";
        popup.setHTML(h);
       
        m.setPopup(popup);
        return m;
    }
    this.removeMarker = function (m) 
    {
        if (m) m.remove(); 
        m = null;
    }
    this.showLayer=function (layerName,showf)
    {
        //var visibility = map.getLayoutProperty(clickedLayer, 'visibility');            //https://docs.mapbox.com/mapbox-gl-js/example/toggle-layers/
        _map.setLayoutProperty(layerName, 'visibility', showf ? 'visible' : 'none');
        var color='#000000';  // Black
        if(!showf)
        {
            // _map.setPaintProperty('road_motorway_casing', 'line-color', '#2c7fb8'); // change the line color
            
        } else {
            color = '#000000';
        }
        _map.setPaintProperty('road_label', 'text-color', color);
    }
    this.switchLayer = function (f) {
        var stl = f ? my_style_bright_v8 : my_style_bright_v8_sat;
        _map.setStyle(stl);
    }
    this.FitRouteBounds=function(locs)
    {
        if (locs) {
            var L = locs.length,x,y,Q;
            var lon = -180, //max lon
                lat = -90,  //max lat
                lon1 = 180, //min lon
                lat1 = 90;  //min lat
            for (var i = 0; i < L; i++)
            {
                Q=locs[i];
                x=Q[0];y=Q[1];
                if (x > lon)
                    lon = x;
                if (y > lat)
                    lat = y;
                if (x < lon1)
                    lon1 = x;
                if (y < lat1)
                    lat1 = y;
            }
            _map.fitBounds([
                [lon1, lat1],    // min LgnLat
                [lon, lat]       // max LgnLat
            ], { padding: 80 });
        }
    }
    //--------------------------------------------------------------
    this.showZoom=function(z) {
        var el = ths.id();
        var rv = ths.getScale(el), c = ths.scaleP();

        var ctx = c.getContext("2d");

        ctx.fillStyle = "#ffffff";
        ctx.globalAlpha = 0.8;
        ctx.clearRect(0, 0, c.width, c.height);//ctx.fillRect(0, 0, 400, 10);
        ctx.fillRect(0, 0, c.width, c.height);

        ctx.fillStyle = "#000000";
        ctx.font = "10px New Time Roman";
        ctx.fillText("OSM Map data 2019 by Mikerel", 10, 8);

        var str = "Zoom: " + z.toFixed(2);
        ctx.fillText(str, 170, 8);

        ctx.fillText(rv.distStr, 270, 8);
        var n = rv.numPixels;

        ctx.lineWidth = 2;

        ctx.beginPath();  // do not forget -> Otherwise the lines won't get cleared.

        ctx.moveTo(300, 2);
        ctx.lineTo(300, 7);
        ctx.lineTo(300 + n, 7);
        ctx.lineTo(300 + n, 2);
        ctx.stroke();
    }

    this.getScale=function (el) {
        var rv = { numPixels: -1, distStr: "" };
        var arr = [5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000];
        var width = el.offsetWidth;
        var height = el.offsetHeight;
        //console.log(dpi_x, dpi_y);
        var P = { x: 0, y: height / 2 };
        var ll = ths.map().unproject(P);

        var P1 = { x: width, y: height / 2 };
        var ll1 = ths.map().unproject(P1);
        if (ll && ll1) {
            var dist = ths.distance(ll.lat, ll.lng, ll1.lat, ll1.lng, "K");
            if (dist != 0) {
                var mpp = dist * 1000 / width;   // meters / pixel
                for (var i = 0; i < 17; i++) {
                    var n = 0, k = arr[i];
                    n = parseInt((k / mpp).toFixed());
                    if (n <= 80 && n >= 40) {
                        rv.numPixels = n;
                        if (k < 1000)
                            rv.distStr = k + "m";
                        else
                            rv.distStr = (k / 1000) + "km";
                    }
                }
            }
        }
        return rv;
    }
    this.distance=function (lat1, lon1, lat2, lon2/*, unit*/) {
        var k = Math.PI / 180;
        var rlat1 = k * lat1, rlat2 = k * lat2;

        //var radlon1 = Math.PI * lon1 / 180
        // var radlon2 = Math.PI * lon2 / 180
        var rtheta = k * (lon1 - lon2);

        var dist = Math.sin(rlat1) * Math.sin(rlat2) + Math.cos(rlat1) * Math.cos(rlat2) * Math.cos(rtheta);
        dist = Math.acos(dist)
        dist = dist / k
        dist = dist * 60 * 1.1515
        //if (unit == "K") { dist = dist * 1.609344 }
        //if (unit == "N") { dist = dist * 0.8684 }
        return (dist * 1.609344)
    }

}