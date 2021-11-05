var my_style_bright_v8 =
{
  "version": 8,
  "name": "Bright",
  
  "sources": {
    "mapbox": {
     
      "type": "vector",
        "tiles": ["http://192.168.0.20/tiles/{z}/{x}/{y}.pbf"],
      //"tiles": ["http://localhost/tiles/{z}/{x}/{y}.pbf"],
     
     //   "tiles": ["https://mikerel.com/tiles/{z}/{x}/{y}.pbf"],
	 //"url": "mapbox://mapbox.mapbox-streets-v7",
     // "minzoom": 1,
     // "maxzoom": 14

    }
  },
   
    "glyphs": "http://192.168.0.20/fonts/mikerel/{fontstack}/{range}.pbf",
    "sprite": "http://192.168.0.20/sprite/sprite",

  //"glyphs": "http://localhost/fonts/mikerel/{fontstack}/{range}.pbf",
  //"sprite": "http://localhost/sprite/sprite",

   // "glyphs": "https://mikerel.com/fonts/mikerel/{fontstack}/{range}.pbf",
   // "sprite": "https://mikerel.com/sprite/sprite",


    "layers": [
  
    {
      "id": "background",
      "type": "background",
      "paint": {
       // "background-color": "#f8f4f0"
          "background-color": "#dddddd"  // light grey
        
      },
      "interactive": true
    },
   
    {
      "id": "landuse_park",
      "type": "fill",
      "source": "mapbox",
      "source-layer": "landuse",
      "filter": [
        "==",
        "class",
         "park"
         
      ],
      "paint": {
        "fill-color": "#d8e8c8"
        //  "fill-color": "#f00000"
      },
       "interactive": true
      },
     {
      "id": "landuse_school",
      "type": "fill",
      "source": "mapbox",
      "source-layer": "landuse",
      "filter": [
        "==",
        "class",
        "school"
      ],
      "paint": {
        "fill-color": "#f0e8f8"
      },
      
      "interactive": true
    },
        

    {
      "interactive": true,
      "layout": {
        "line-cap": "round"
      },
     
      "filter": [
        "all",
        [
          "!=",
          "class",
          "river"
        ],
        [
          "!=",
          "class",
          "stream"
        ],
        [
          "!=",
          "class",
          "canal"
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "waterway",
      "paint": {
        "line-color": "#a0c8f0",
        "line-width": {
          "base": 1.3,
          "stops": [
            [
              13,
              0.5
            ],
            [
              20,
              2
            ]
          ]
        }
      },
      "source-layer": "waterway"
      },

    {
      "interactive": true,
      "layout": {
        "line-cap": "round"
      },
     
      "filter": [
        "in",
        "class",
          "river",
        "stream"
      ],
      "type": "line",
      "source": "mapbox",
      "id": "waterway_river",
      "paint": {
        "line-color": "#a0c8f0",
        "line-width": {
          "base": 1.2,
          "stops": [
            [
              11,
              0.5
            ],
            [
              20,
              6
            ]
          ]
        }
      },
      "source-layer": "waterway"
      },
    
    // waterway riverbank
      {
          "interactive": true,
          
          "filter": [
              "all",
              [
                  "!=",
                  "$type",
                  "LineString"
              ],
              [
              "in",
              "class",
                  "riverbank", "canal"
              ]
          ],
          "type": "fill",
          "source": "mapbox",
          "id": "waterway_riverbank",
          "paint": {
              "fill-color": "#a0c8f0",
             // "fill-color": "#ff0000",
              // "fill-opacity": 0.7 
          },
          "source-layer": "waterway"
      },
      {
          "interactive": true,

          "filter": [
              "all",
              [
                  "==",
                  "$type",
                  "LineString"
              ],
              [
              "==",
              "class",
                  "riverbank"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "waterway_riverbank_str",
          "paint": {
              // "fill-color": "#a0c8f0",
             // "fill-color": "#ff0000",
              // "fill-opacity": 0.7 

              "line-color": "#0000ff",
          },
          "source-layer": "waterway"
      },
     

    //svk lakes,pond,reservoir, etc
    {
      "id": "water",
      "type": "fill",
      "source": "mapbox",
      "source-layer": "water",
      "paint": {
        "fill-color": "#a0c8f0",
       //   "fill-color": "#ff0000",
         // "fill-opacity": 0.7 
      },
      
      "interactive": true
    },

      {
          "id": "landuse_wood",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
              "in",
              "class",
              "wood", "wetland", "grass", "rock"
          ],
          "paint": {
              "fill-color": "#6a4",
              "fill-opacity": 0.15        // it was 0.2

          },

          "interactive": true
      },

      //svk leisure , type: park
      {
          "id": "leisure_landuse",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
                  "in",
                  "class",
                  "lei_park",
                  "lei_nature_reserve",
                  "lei_golf_course"
              
          ],
          "paint": {
              "fill-color": "#33ff33",
             // "fill-color": "#ff0000",
               "fill-opacity": 0.1 
          },

          "interactive": true
      },

      //svk landuse -> agriculture
      {
          "id": "leisure_agriculture",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
                  "==",
                  "class",
              "agriculture"
             
          ],
          "paint": {
             // "fill-color": "#33ff33",
              "fill-color": "#33ff33",
              "fill-opacity": 0.3
          },

          "interactive": true
      },
      //svk landuse -> industrial
      {
          "id": "landuse_industrial",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
              "in",
              "class",
              "industrial","residential"

          ],
          "paint": {
              "fill-color": "#FFF8DC",    //brown
             // "fill-color": "#ff77ff",
              "fill-opacity": 0.2
          },

          "interactive": true
      },

      //svk sport , type:soccer 
      {
          "id": "leisure_sport",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
             
                  "in",
                  "class",
                  "sport_soccer","sport_tennis"
              
          ],
          "paint": {
              "fill-color": "#d8e8c8",
              "fill-opacity": 0.6
          },

          "interactive": true
      },
      //// svk added for parking
      //{
      //    "id": "landuse_parking",
      //    "type": "fill",
      //    "source": "mapbox",
      //    "source-layer": "landuse",
      //    "filter": [
      //        "==",
      //        "class",
      //        "parking"
      //    ],
      //    "paint": {
      //        //"fill-color": "#cccccc",
      //        "fill-color": "#ff0000",
      //        // "fill-opacity": 0.9
      //    },

      //    "interactive": true
      //},



      // svk added for island -> must be drawn after water  !!!
      {
          "id": "landuse_island",
          "type": "fill",
          "source": "mapbox",
          "source-layer": "landuse",
          "filter": [
              "==",
              "class",
              "island"
          ],
          "paint": {
              "fill-color": "#cccccc", //"fill-color": "#d8e8c8"
              // "fill-opacity": 0.9
          },
         
          "interactive": true
      },
// the following two shows water wave-like
    //{
    //  "id": "water_offset",
    //  "paint": {
    //    "fill-color": "white",
    //    "fill-opacity": 0.3,
    //    "fill-translate": [
    //      0,
    //      2.5
    //    ]
    //  },
      
    //  "interactive": true,
    //  "ref": "water"
    //},
    //{
    //  "id": "water_pattern",
    //  "paint": {
    //    "fill-translate": [
    //      0,
    //      2.5
    //    ],
    //    "fill-pattern": "wave"
    //  },
     
    //  "interactive": true,
    //  "ref": "water"
    //  },

      


    {
      "interactive": true,
      "minzoom": 11,
      
      "filter": [
        "==",
        "$type",
        "Polygon"
      ],
      "type": "fill",
      "source": "mapbox",
      "id": "aeroway_fill",
      "paint": {
          "fill-color": "#ccccff",
        "fill-opacity": 0.7
      },
      "source-layer": "aeroway"
      },

    {
      "interactive": true,
      "minzoom": 11,
      
      "filter": [
          "==",
          "class",
          "runway"
      ],
      "type": "line",
      "source": "mapbox",
      "id": "aeroway_runway",
      "paint": {
        "line-color": "#f0ede9",
        "line-width": {
          "base": 1.2,
          "stops": [
            [
              11,
              3
            ],
            [
              20,
              16
            ]
          ]
        }
      },
      "source-layer": "aeroway"
    },
    {
      "interactive": true,
      "minzoom": 11,
      
      "filter": [
          "==",
          "class",
          "taxiway"
        
      ],
      "type": "line",
      "source": "mapbox",
      "id": "aeroway_taxiway",
      "paint": {
        "line-color": "#f0ede9",
        "line-width": {
          "base": 1.2,
          "stops": [
            [
              11,
              0.5
            ],
            [
              20,
              6
            ]
          ]
        }
      },
      "source-layer": "aeroway"
      },
// building=================
    {
      "id": "building",
      "type": "fill",
      "source": "mapbox",
      "source-layer": "building",
      "paint": {
        "fill-color": {
          "base": 1,
          "stops": [
            [
              15.5,
              "#f2eae2"
            ],
            [
              16,
              "#dfdbd7"
            ]
          ]
        }
      },
     
      "interactive": true
    },
    
    {
      "id": "building_top",
      "paint": {
        "fill-color": "#f2eae2",
        "fill-opacity": {
          "base": 1,
          "stops": [
            [
              15,
              0
            ],
            [
              16,
              1
            ]
          ]
        },
        "fill-translate": {
          "stops": [
            [
              15,
              [
                0,
                0
              ]
            ],
            [
              16,
              [
                -2,
                -2
              ]
            ]
          ],
          "base": 1
        },
        "fill-outline-color": "#dfdbd7"
      },
      
      "interactive": true,
      "ref": "building"
      },
      
    // building=================End


// motorway link 

      {
          "interactive": true,
          "minzoom": 8,
          "layout": {
              "line-cap": "round",
              "line-join": "round"
          },

          "filter": [
              "all",
              [
                  "==",
                  "class",
                  "motorway_link"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_motorway_link_casing",
          "paint": {
              "line-color": "#e9ac77",
              "line-width": {
                  "base": 2.2,
                  "stops": [
                      [
                          12,
                          2.4
                      ],
                      [
                          13,
                          4
                      ],
                      [
                          14,
                          5
                      ],
                      [
                          20,
                          15
                      ]
                  ]
              },
              "line-opacity": 1
          },
          "source-layer": "road"
      },

      // for minor roads 
      {
          "interactive": true,
          "minzoom": 14,                 // svk hide service/track types below zoom 14
          "layout": {
              "line-cap": "round",
              "line-join": "round"
          },

          "filter": [
              "all",
              [
                  "in",
                  "class",
                  "service",
                  "track",
                  "path"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_service_track_casing",
          "paint": {
                "line-color": "#cfcdca",
			    "line-width": {
                  "base": 1.2,
                  "stops": [
                      [
                          15,
                          1
                      ],
                      [
                          16,
                          4
                      ],
                      [
                          20,
                          11
                      ]
                  ]
              }
          },
          "source-layer": "road"
      },

      //--------- major=2 roads

      {
          "interactive": true,
          "layout": {
              "line-cap": "round",
              "line-join": "round"
          },

          "filter": [
              "all",
              [
                  "==",
                  "$type",
                  "LineString"
              ],
              [
                  "all",
                  [
                      "in",
                      "class",
                      "residential",          // corrected for minor roads 
                        "road",
                        "unclassified",
                      "tertiary" 
                  ],
                  [
                      "!in",
                      "structure",
                      "bridge",
                      "tunnel"
                  ]
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_street_casing",
          "paint": {
              "line-color": "#f0f0f0",
              "line-width": {
                  "base": 1.2,
                  "stops": [
                      [
                          12,
                          0.5
                      ],
                      [
                          13,
                          1
                      ],
                      [
                          14,
                          4
                      ],
                      [
                          20,
                          15
                      ]
                  ]
              },
              "line-opacity": {
                  "stops": [
                      [
                          12,
                          0
                      ],
                      [
                          12.5,
                          1
                      ]
                  ]
              }
          },
          "source-layer": "road"
      },

      // for secondary , tertiary
      {
          "interactive": true,
          "layout": {
              "line-cap": "round",
              "line-join": "round",
              "visibility": "visible"
          },

          "filter": [
              "all",
              [
                  "in",
                  "class",
                  "secondary",
                  "tertiary"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_secondary_tertiary_casing",
          "paint": {
              "line-color": "#e9ac77",
              "line-width": {
                  "base": 1.2,
                  "stops": [
                      [
                          8,
                          1.5
                      ],
                      [
                          20,
                          17
                      ]
                  ]
              },
              "line-opacity": 0.4
          },
          "source-layer": "road"
      },

      // for trunk, primary
      {
          "interactive": true,
          "layout": {
              "line-cap": "round",
              "line-join": "round",
              "visibility": "visible"
          },

          "filter": [
              "all",
              [
                  "in",
                  "class",
                  "trunk",
                  "primary"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_trunk_primary_casing",
          "paint": {
              "line-color": "#e9ac77",
              "line-width": {
                  "base": 1.2,
                  "stops": [
                      [
                          5,
                          0.4
                      ],
                      [
                          6,
                          0.6
                      ],
                      [
                          7,
                          1.5
                      ],
                      [
                          20,
                          22
                      ]
                  ]
              },
              "line-opacity": 1
          },
          "source-layer": "road"
      },

      // for motorway
      {
          "interactive": true,
          "minzoom": 4,
          "layout": {
              "line-cap": "round",
              "line-join": "round",
              "visibility": "visible"
          },

          "filter": [
              "all",
              [
                  "==",
                  "class",
                  "motorway"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "type": "line",
          "source": "mapbox",
          "id": "road_motorway_casing",
          "paint": {
              "line-color": "#e9ac77",
              "line-width": {
                  "base": 1.2,
                  "stops": [
                      [
                          5,
                          0.4
                      ],
                      [
                          6,
                          0.6
                      ],
                      [
                          7,
                          1.5
                      ],
                      [
                          20,
                          22
                      ]
                  ]
              }
          },
          "source-layer": "road"
      },

      // for railroads

      {
          "id": "road_major_rail",
          "type": "line",
          "source": "mapbox",
          "source-layer": "road",
          "filter": [
              "all",
              [
                  "==",
                  "class",
                  "rail"
              ],
              [
                  "!in",
                  "structure",
                  "bridge",
                  "tunnel"
              ]
          ],
          "paint": {
              "line-color": "#bbb",
              "line-width": {
                  "base": 1.4,
                  "stops": [
                      [
                          14,
                          0.4
                      ],
                      [
                          15,
                          0.75
                      ],
                      [
                          20,
                          2
                      ]
                  ]
              }
          },

          "interactive": true
      },




   

    // water label for point -> lake, pond , etc
    {
        "interactive": true,
        "minzoom": 8,
      "layout": {
        "text-font": [
        //  "Open Sans Italic",
          "Arial Unicode MS Regular"
        ],
        "text-field": "{name_en}",
        "text-max-width": 5,
          "text-size": 11
      },
      
      "filter": [
        "==",
        "$type",
        "Point"
      ],
      "type": "symbol",
      "source": "mapbox",
      "id": "water_label",
      "paint": {
          "text-color": "#606000",
      },
      "source-layer": "water_label"
      },
      // water for stringline -> river , etc
      {
          "interactive": true,
          "minzoom": 8,
          "layout": {
              "text-font": [
                //  "Open Sans Italic",
                  "Arial Unicode MS Regular"
              ],
              "text-field": "{name_en}",
              "text-max-width": 5,
              "text-size": 11,
              "symbol-placement": "line"
          },

          "type": "symbol",
          "source": "mapbox",
          "id": "water_label_river",
          "paint": {
              "text-color": "#000066",
          },
          "source-layer": "water_label"
      },
   
    //svk added for label -> island
      {
          "interactive": true,
          "minzoom": 8,
          "layout": {
             
              "text-font": [
                 // "Open Sans Semibold",
                  "Arial Unicode MS Bold"
              ],
              "text-field": "{name_en}",
              "text-max-width": 9,
              "text-padding": 2,
              "text-offset": [
                  0,
                  0.6
              ],
              "text-anchor": "top",
              "text-size": 12
          },
         
          "filter": [
              "in",
              "class",
              "island",
              "park","bay"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_island",
          "paint": {
              "text-color": "#003366",
          },
          "source-layer": "place_label"
      },

      //svk added for label -> building + others
      {
          "interactive": true,
          "minzoom": 13,
          "layout": {

              "text-font": [
                 // "Open Sans Semibold",
                 // "Arial Unicode MS Bold"
                  "Arial Unicode MS"
              ],
              "text-field": "{name_en}",
              //"text-max-width": 9,
              //"text-padding": 2,
              //"text-offset": [
              //    0,
              //    0.6
              //],
              "text-anchor": "top",
              "text-size": 12
          },

          "filter": [
              "in",
              "class",   //"hotel","hospital" ,"school", "university",  "public","government" ,"commercial","office","industrial","retail","residential","warehouse"
              "hotel", "hospital", "school", "university", "public", "government", "commercial", "office", "industrial", "retail", "residential", "warehouse","apartments",
              "fuel", "cafe", "amenity"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_building",
          "paint": {
              "text-color": "#0059b3",
          },
          "source-layer": "place_label"
      },
/*
    {
      "interactive": true,
      "minzoom": 11,
      "layout": {
        "icon-image": "{maki}-11",
        "text-font": [
          "Open Sans Semibold",
          "Arial Unicode MS Bold"
        ],
        "text-field": "{name_en}",
        "text-max-width": 9,
        "text-padding": 2,
        "text-offset": [
          0,
          0.6
        ],
        "text-anchor": "top",
        "text-size": 12
      },
     
      "filter": [
        "all",
        [
          "==",
          "$type",
          "Point"
        ],
        [
          "in",
          "scalerank",
          1,
          2,
          3
        ]
      ],
      "type": "symbol",
      "source": "mapbox",
      "id": "airport_label",
      "paint": {
        "text-color": "#666",
        "text-halo-color": "#ffffff",
        "text-halo-width": 1,
        "text-halo-blur": 0.5
      },
      "source-layer": "airport_label"
      },
*/
    {
        "interactive": true,
       
      "layout": {
        "text-field": "{name_en}",
        "text-font": [
          "Open Sans Regular",
          "Arial Unicode MS Regular"
        ],
        "text-size": {
          "base": 1,
          "stops": [
            [
              12,
              11
            ],
            [
              13,
              12
            ]
          ]
        },
        "symbol-placement": "line"
      },
     
      "filter": [
        "!=",
        "class",
        "ferry"
      ],
      "type": "symbol",
      "source": "mapbox",
      "id": "road_label",
        "paint": {
            "text-color": "#222222",
       // "text-color": "#765",
       // "text-halo-width": 1,
       // "text-halo-blur": 0.5
      },
      "source-layer": "road_label"
      },

    //shield roads -> motorway,primary,trunk  =================================================
    {
      "interactive": true,
      "minzoom": 4,
        "layout": {
            "text-field": "{ref}",
            "text-font": [
               "Arial Unicode MS Bold"
            ],
            "text-size": 11,
            "icon-image": "motorway_{reflen}",

            "symbol-placement": "point",

        "symbol-spacing": {
            "base": 500,
            "stops": [
              [8, 3500],
              [9, 3000],
              [10,2500],
              [11, 2000],
              [12, 1500],
              [13, 1000],
              [14, 500]
            ]
        },

        "text-rotation-alignment": "viewport",
        "icon-rotation-alignment": "viewport"
      },
      
        "filter": [
            "all",
            [
                "==",
                "$type",
                "Point"           // we show here all shields which were written to the layer as Point !!!
            ],
             [
                "in",
                "class",
                 "motorway",
                 "primary",
                "trunk"
            ],
            [
               "<=",
               "reflen",
               6
            ]  
        ],
      "type": "symbol",
      "source": "mapbox",
      "id": "road_lbl_hw_shld",
      "paint": { },
      "source-layer": "road_label"
      },   //========================================================================

      //shield roads -> major=2
      {
          "interactive": true,
          "minzoom": 10,
          "layout": {
              "text-field": "{ref}",
              "text-font": [
                  "Open Sans Semibold",
                  "Arial Unicode MS Bold"
              ],
              "text-size": 11,
              "icon-image": "motorway_{reflen}",

              "symbol-placement": "line",

              "symbol-spacing": {
                  "base": 500,
                  "stops": [
                     
                      [10, 2500],
                      [11, 2000],
                      [12, 1500],
                      [13, 1000],
                      [14, 500]
                  ]
              },

              "text-rotation-alignment": "viewport",
              "icon-rotation-alignment": "viewport"
          },

          "filter": [
              "all",
              [
                  "in",
                  "class",
                  "secondary",
                  "residential",
                  "tertiary",
                  "road"
              ],
              [
                  "<=",
                  "reflen",
                  6
              ]
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "road_lbl_hw_shld_1",
          "paint": {},
          "source-layer": "road_label"
      },
   
    {
        "interactive": true,
        "minzoom": 2,
        "maxzoom": 12,
      "layout": {
        "text-font": [
          "Open Sans Semibold",
          "Arial Unicode MS Bold"
        ],
        "text-field": "{name_en}",
        "text-max-width": 8,
        "text-size": {
          "base": 1.2,
          "stops": [
            [
              7,
              14
            ],
            [
              11,
              18
            ]
          ]
        }
      },
     
      "filter": [
        "<=",
        "rank",
        5
      ],
      "type": "symbol",
      "source": "mapbox",
      "id": "place_label_city",
      "paint": {
        "text-color": "#333",
        "text-halo-color": "rgba(255,255,255,0.8)",
        "text-halo-width": 1.2
      },
      "source-layer": "place_label"
    },
    //===========fuel label================
      {
          "interactive": true,
          "layout": {
              "text-font": [
                 "Arial Unicode MS Bold"
              ],
              "text-field": "{name_en}",
              "text-max-width": 8,
              "text-size": 11,
             // "icon-image": "fuel-15",
              "icon-image": "fillingstation",
              "text-offset":[0,2.2],
              "symbol-placement": "point"
          },

          "filter": [
              "==",
              "class",
              "fuel"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_fuel",
          
          "paint": {
              "text-color": "brown"
             
          },
          "source-layer": "place_label"
      },

      //===========service center label================
      {
          "interactive": true,
          "layout": {
              "text-font": [
                  "Arial Unicode MS Bold"
              ],
              "text-field": "{name_en}",
              "text-max-width": 8,
              "text-size": 11,
              "icon-image": "restaurant",
              "text-offset": [0, 1.2],
              "symbol-placement": "point"
          },

          "filter": [
              "==",
              "class",
              "services"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_services",
          "paint": {
              "text-color": "#5555ff"

          },
          "source-layer": "place_label"
      },

      //  "lei_nature_reserve", "lei_golf_course","lei_park"
      {
          "interactive": true,
          "layout": {
              "text-font": [
                  "Arial Unicode MS Bold"
              ],
              "text-field": "{name_en}",
              "text-size": 12,
          },

          "filter": [
              "in",
              "class",
              "lei_nature_reserve", "lei_golf_course", "lei_park"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_lei",
          "paint": {
              //"text-color": "#ff0000"
              "text-color": "#003366"

          },
          "source-layer": "place_label"
      },


      //===========aerodrome (airport) label================
      {
          "interactive": true,
          "layout": {
              "text-font": [
                  "Arial Unicode MS Bold"
              ],
              "text-field": "{name_en}",
              "text-max-width": 8,
              "text-size": 11,
              "icon-image": "airport",
              "text-offset": [0, 3.6],
              "symbol-placement": "point"
          },

          "filter": [
              "==",
              "class",
              "aerodrome"
          ],
          "type": "symbol",
          "source": "mapbox",
          "id": "place_label_airport",
          "paint": {
              "text-color": "#5555ff"

          },
          "source-layer": "place_label"
      },

      {
      "interactive": true,
	  "minzoom": 5,
      "layout": {
        "text-font": [
          "Open Sans Bold",
          "Arial Unicode MS Bold"
        ],
        "text-transform": "uppercase",
        "text-letter-spacing": 0.1,
        "text-field": "{name_en}",
        "text-max-width": 9,
        "text-size": {
          "base": 1.2,
          "stops": [
            [
              1,
              6
            ],
            [
              8,
              8
            ]
          ]
        }
      },
      
      /*"filter": [
        ">",
        "rank",
        5
        
      ],*/
      "type": "symbol",
      "source": "mapbox",
      "id": "place_label_other",
      "paint": {
        "text-color": "#633",
        "text-halo-color": "rgba(255,255,255,0.8)",
        "text-halo-width": 1.2
      },
      "source-layer": "place_label"
    },
   // state_label
   {
      "interactive": true,
	   "minzoom": 4,
	   
      "layout": {
        "text-font": [
          "Open Sans Bold",
          "Arial Unicode MS Bold"
        ],
        "text-transform": "uppercase",
        "text-letter-spacing": 0.1,
        "text-field": "{name_en}",
        "text-max-width": 9,
        "text-size": {
          "base": 1.2,
          "stops": [
            [
              4,
              6
            ],
            [
              8,
              8
            ]
          ]
        }
      },
      
      /*"filter": [
        ">",
        "rank",
        5
        
      ],*/
      "type": "symbol",
      "source": "mapbox",
      "id": "state_label_id",
      "paint": {
        "text-color": "#633",
        "text-halo-color": "rgba(255,255,255,0.8)",
        "text-halo-width": 1.2
      },
      "source-layer": "state_label"
    },
   //poi label
   {
      "interactive": true,
	   "minzoom": 6,
	  
      "layout": {
        "text-font": [
          "Open Sans Bold"
        ],
        "text-transform": "uppercase",
        "text-letter-spacing": 0.1,
        "text-field": "{name_en}",
        "text-max-width": 9,
        "text-size": {
          "base": 1.2,
          "stops": [
            [
              1,
              6
            ],
            [
              8,
              8
            ]
          ]
        }
      },
      
      /*"filter": [
        ">",
        "rank",
        5
        
      ],*/
      "type": "symbol",
      "source": "mapbox",
      "id": "poi_label_id",
      "paint": {
        "text-color": "#633",
        "text-halo-color": "rgba(255,255,255,0.8)",
        "text-halo-width": 1.2
      },
      "source-layer": "poi_label"
    },
	//country
    {
      "interactive": true,
	  "minzoom": 1,
	  "maxzoom": 6,
      "layout": {
        "text-font": [
          "Open Sans Bold"
        ],
        "text-transform": "uppercase",
        "text-letter-spacing": 0.1,
        "text-field": "{name_en}",
        "text-max-width": 9,
        "text-size": {
          "base": 1.2,
          "stops": [
            [
              1,
              8
            ],
            [
              8,
              10
            ]
          ]
        }
      },
      
      /*"filter": [
        ">",
        "rank",
        5
        
      ],*/
      "type": "symbol",
      "source": "mapbox",
      "id": "country_label_id",
      "paint": {
        "text-color": "#633",
        "text-halo-color": "rgba(255,255,255,0.8)",
        "text-halo-width": 1.2
      },
      "source-layer": "country_label"
    },
   //admin layer
       {
      "interactive": true,
      "layout": {
        "line-join": "round"
      },
     
      "filter": [
        "all",
        [
          ">=",
          "admin_level",
          3
        ],
        [
          "==",
          "maritime",
          0
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "admin_level_3",
      "paint": {
        "line-color": "#9e9cab",
        "line-dasharray": [
          3,
          1,
          1,
          1
        ],
        "line-width": {
          "base": 1,
          "stops": [
            [
              4,
              0.4
            ],
            [
              5,
              1
            ],
            [
              12,
              3
            ]
          ]
        }
      },
      "source-layer": "admin"
    },
    {
      "interactive": true,
      "layout": {
        "line-join": "round",
        "line-cap": "round"
      },
     
      "filter": [
        "all",
        [
          "==",
          "admin_level",
          2
        ],
        [
          "==",
          "disputed",
          0
        ],
        [
          "==",
          "maritime",
          0
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "admin_level_2",
      "paint": {
        "line-color": "#9e9cab",
        "line-width": {
          "base": 1,
          "stops": [
            [
              4,
              1
            ],
            [
              5,
              1.1
            ],
            [
              12,
              1.2
            ]
          ]
        }
      },
      "source-layer": "admin"
    },
    {
      "interactive": true,
      "layout": {
        "line-cap": "round"
      },
     
      "filter": [
        "all",
        [
          "==",
          "admin_level",
          2
        ],
        [
          "==",
          "disputed",
          1
        ],
        [
          "==",
          "maritime",
          0
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "admin_level_2_disputed",
      "paint": {
        "line-color": "#9e9cab",
        "line-dasharray": [
          2,
          2
        ],
        "line-width": {
          "base": 1,
          "stops": [
            [
              4,
              1.4
            ],
            [
              5,
              2
            ],
            [
              12,
              8
            ]
          ]
        }
      },
      "source-layer": "admin"
    },
    {
      "interactive": true,
      "layout": {
        "line-join": "round"
      },
      
      "filter": [
        "all",
        [
          ">=",
          "admin_level",
          3
        ],
        [
          "==",
          "maritime",
          1
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "admin_level_3_maritime",
      "paint": {
        "line-color": "#a0c8f0",
        "line-opacity": 0.5,
        "line-dasharray": [
          3,
          1,
          1,
          1
        ],
        "line-width": {
          "base": 1,
          "stops": [
            [
              4,
              0.4
            ],
            [
              5,
              1
            ],
            [
              12,
              3
            ]
          ]
        }
      },
      "source-layer": "admin"
    },
    {
      "interactive": true,
      "layout": {
        "line-cap": "round"
      },
     
      "filter": [
        "all",
        [
          "==",
          "admin_level",
          2
        ],
        [
          "==",
          "maritime",
          1
        ]
      ],
      "type": "line",
      "source": "mapbox",
      "id": "admin_level_2_maritime",
      "paint": {
        "line-color": "#a0c8f0",
        "line-opacity": 0.5,
        "line-width": {
          "base": 1,
          "stops": [
            [
              4,
              1.4
            ],
            [
              5,
              2
            ],
            [
              12,
              8
            ]
          ]
        }
      },
      "source-layer": "admin"
      },



  ]
};
