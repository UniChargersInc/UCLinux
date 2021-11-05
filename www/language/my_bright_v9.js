var my_style_bright_v9 =
{
    "version": 8,
    "name": "Bright",
    "metadata": {
        "mapbox:autocomposite": true,
        "mapbox:type": "template",
        "mapbox:groups": {
            "1444849364238.8171": {
                "name": "Buildings",
                "collapsed": true
            },
            "1444849354174.1904": {
                "name": "Tunnels",
                "collapsed": true
            },
            "1444849320558.5054": {
                "name": "Water labels",
                "collapsed": true
            },
            "1444849371739.5945": {
                "name": "Aeroways",
                "collapsed": true
            },
            "1444849258897.3083": {
                "name": "Marine labels",
                "collapsed": true
            },
            "1444849388993.3071": {
                "name": "Landuse",
                "collapsed": true
            },
            "1444849242106.713": {
                "name": "Country labels",
                "collapsed": true
            },
            "1444849382550.77": {
                "name": "Water",
                "collapsed": true
            },
            "1444849345966.4436": {
                "name": "Roads",
                "collapsed": true
            },
            "1444849307123.581": {
                "name": "Admin  lines",
                "collapsed": true
            },
            "1456163609504.0715": {
                "name": "Road labels",
                "collapsed": true
            },
            "1444849272561.29": {
                "name": "Place labels",
                "collapsed": true
            },
            "1444849290021.1838": {
                "name": "Road labels",
                "collapsed": true
            },
            "1444849334699.1902": {
                "name": "Bridges",
                "collapsed": true
            },
            "1444849297111.495": {
                "name": "POI labels",
                "collapsed": true
            }
        }
    },
    "sources": {
        "mapbox": {
           // "url": "mapbox://mapbox.mapbox-streets-v7",
		    "tiles": ["http://192.168.0.20/tiles/{z}/{x}/{y}.pbf"],
            "type": "vector"
        }
    },
    "center": [
        -118.2518,
        34.0442
    ],
    "zoom": 15,
    //"sprite": "mapbox://sprites/mapbox/bright-v9",
    //"glyphs": "mapbox://fonts/mapbox/{fontstack}/{range}.pbf",
	 "glyphs": "http://192.168.0.20/fonts/mikerel/{fontstack}/{range}.pbf",
    "sprite": "http://192.168.0.20/sprite/sprite",
    "layers": [
        {
            "id": "background",
            "type": "background",
            "paint": {
                "background-color": "#f8f4f0"
            },
            "interactive": true
        },
        {
            "id": "landuse_overlay_national_park",
            "type": "fill",
            "source": "mapbox",
            "source-layer": "landuse_overlay",
            "filter": [
                "==",
                "class",
                "national_park"
            ],
            "paint": {
                "fill-color": "#d8e8c8",
                "fill-opacity": 0.75
            },
            "metadata": {
                "mapbox:group": "1444849388993.3071"
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
            },
            "metadata": {
                "mapbox:group": "1444849388993.3071"
            },
            "interactive": true
        },
        {
            "id": "landuse_cemetery",
            "type": "fill",
            "source": "mapbox",
            "source-layer": "landuse",
            "filter": [
                "==",
                "class",
                "cemetery"
            ],
            "paint": {
                "fill-color": "#e0e4dd"
            },
            "metadata": {
                "mapbox:group": "1444849388993.3071"
            },
            "interactive": true
        },
        {
            "id": "landuse_hospital",
            "type": "fill",
            "source": "mapbox",
            "source-layer": "landuse",
            "filter": [
                "==",
                "class",
                "hospital"
            ],
            "paint": {
                "fill-color": "#fde"
            },
            "metadata": {
                "mapbox:group": "1444849388993.3071"
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
            "metadata": {
                "mapbox:group": "1444849388993.3071"
            },
            "interactive": true
        },
        {
            "id": "landuse_wood",
            "type": "fill",
            "source": "mapbox",
            "source-layer": "landuse",
            "filter": [
                "==",
                "class",
                "wood"
            ],
            "paint": {
                "fill-color": "#6a4",
                "fill-opacity": 0.1
            },
            "metadata": {
                "mapbox:group": "1444849388993.3071"
            },
            "interactive": true
        },
        {
            "interactive": true,
            "layout": {
                "line-cap": "round"
            },
            "metadata": {
                "mapbox:group": "1444849382550.77"
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
            "metadata": {
                "mapbox:group": "1444849382550.77"
            },
            "filter": [
                "==",
                "class",
                "river"
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
        {
            "interactive": true,
            "layout": {
                "line-cap": "round"
            },
            "metadata": {
                "mapbox:group": "1444849382550.77"
            },
            "filter": [
                "in",
                "class",
                "stream",
                "canal"
            ],
            "type": "line",
            "source": "mapbox",
            "id": "waterway_stream_canal",
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
                            6
                        ]
                    ]
                }
            },
            "source-layer": "waterway"
        },
        {
            "id": "water",
            "type": "fill",
            "source": "mapbox",
            "source-layer": "water",
            "paint": {
                "fill-color": "#a0c8f0"
            },
            "metadata": {
                "mapbox:group": "1444849382550.77"
            },
            "interactive": true
        },
        {
            "id": "water_offset",
            "paint": {
                "fill-color": "white",
                "fill-opacity": 0.3,
                "fill-translate": [
                    0,
                    2.5
                ]
            },
            "metadata": {
                "mapbox:group": "1444849382550.77"
            },
            "interactive": true,
            "ref": "water"
        },
        {
            "id": "water_pattern",
            "paint": {
                "fill-translate": [
                    0,
                    2.5
                ],
                "fill-pattern": "wave"
            },
            "metadata": {
                "mapbox:group": "1444849382550.77"
            },
            "interactive": true,
            "ref": "water"
        },
        {
            "interactive": true,
            "minzoom": 11,
            "metadata": {
                "mapbox:group": "1444849371739.5945"
            },
            "filter": [
                "==",
                "$type",
                "Polygon"
            ],
            "type": "fill",
            "source": "mapbox",
            "id": "aeroway_fill",
            "paint": {
                "fill-color": "#f0ede9",
                "fill-opacity": 0.7
            },
            "source-layer": "aeroway"
        },
        {
            "interactive": true,
            "minzoom": 11,
            "metadata": {
                "mapbox:group": "1444849371739.5945"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    "==",
                    "type",
                    "runway"
                ]
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
            "metadata": {
                "mapbox:group": "1444849371739.5945"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    "==",
                    "type",
                    "taxiway"
                ]
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
            "metadata": {
                "mapbox:group": "1444849364238.8171"
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
            "metadata": {
                "mapbox:group": "1444849364238.8171"
            },
            "interactive": true,
            "ref": "building"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "==",
                    "class",
                    "motorway_link"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_motorway_link_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-dasharray": [
                    0.5,
                    0.25
                ],
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "in",
                    "class",
                    "service",
                    "track"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_service_track_casing",
            "paint": {
                "line-color": "#cfcdca",
                "line-dasharray": [
                    0.5,
                    0.25
                ],
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "==",
                    "class",
                    "link"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_link_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "in",
                    "class",
                    "street",
                    "street_limited"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_street_casing",
            "paint": {
                "line-color": "#cfcdca",
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "in",
                    "class",
                    "secondary",
                    "tertiary"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_secondary_tertiary_casing",
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "in",
                    "class",
                    "trunk",
                    "primary"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_trunk_primary_casing",
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "==",
                    "class",
                    "motorway"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "tunnel_motorway_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-dasharray": [
                    0.5,
                    0.25
                ],
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
        {
            "id": "tunnel_path_pedestrian",
            "type": "line",
            "source": "mapbox",
            "source-layer": "road",
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
                        "==",
                        "structure",
                        "tunnel"
                    ],
                    [
                        "in",
                        "class",
                        "path",
                        "pedestrian"
                    ]
                ]
            ],
            "paint": {
                "line-color": "#cba",
                "line-dasharray": [
                    1.5,
                    0.75
                ],
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15,
                            1.2
                        ],
                        [
                            20,
                            4
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "interactive": true
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_motorway_link",
            "paint": {
                "line-color": "#fc8",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "tunnel_motorway_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_service_track",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15.5,
                            0
                        ],
                        [
                            16,
                            2
                        ],
                        [
                            20,
                            7.5
                        ]
                    ]
                }
            },
            "ref": "tunnel_service_track_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_link",
            "paint": {
                "line-color": "#fff4c6",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "tunnel_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_street",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            13.5,
                            0
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                },
                "line-opacity": 1
            },
            "ref": "tunnel_street_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_secondary_tertiary",
            "paint": {
                "line-color": "#fff4c6",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            10
                        ]
                    ]
                }
            },
            "ref": "tunnel_secondary_tertiary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_trunk_primary",
            "paint": {
                "line-color": "#fff4c6",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "tunnel_trunk_primary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "id": "tunnel_motorway",
            "paint": {
                "line-color": "#ffdaa6",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "tunnel_motorway_casing"
        },
        {
            "id": "tunnel_major_rail",
            "type": "line",
            "source": "mapbox",
            "source-layer": "road",
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "tunnel"
                ],
                [
                    "in",
                    "class",
                    "major_rail",
                    "minor_rail"
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
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "interactive": true
        },
        {
            "id": "tunnel_major_rail_hatching",
            "paint": {
                "line-color": "#bbb",
                "line-dasharray": [
                    0.2,
                    8
                ],
                "line-width": {
                    "base": 1.4,
                    "stops": [
                        [
                            14.5,
                            0
                        ],
                        [
                            15,
                            3
                        ],
                        [
                            20,
                            8
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849354174.1904"
            },
            "interactive": true,
            "ref": "tunnel_major_rail"
        },
        {
            "interactive": true,
            "minzoom": 12,
            "layout": {
                "line-cap": "round",
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
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
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-cap": "round",
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "filter": [
                "all",
                [
                    "in",
                    "class",
                    "service",
                    "track"
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
        {
            "interactive": true,
            "minzoom": 13,
            "layout": {
                "line-cap": "round",
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "filter": [
                "all",
                [
                    "==",
                    "class",
                    "link"
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
            "id": "road_link_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-cap": "round",
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
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
                        "street",
                        "street_limited"
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
                "line-color": "#cfcdca",
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
        {
            "interactive": true,
            "layout": {
                "line-cap": "round",
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-cap": "round",
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
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
        {
            "interactive": true,
            "minzoom": 5,
            "layout": {
                "line-cap": "round",
                "line-join": "round",
                "visibility": "visible"
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
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
        {
            "id": "road_path_pedestrian",
            "type": "line",
            "source": "mapbox",
            "source-layer": "road",
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
                        "path",
                        "pedestrian"
                    ],
                    [
                        "!in",
                        "structure",
                        "bridge",
                        "tunnel"
                    ]
                ]
            ],
            "paint": {
                "line-color": "#cba",
                "line-dasharray": [
                    1.5,
                    0.75
                ],
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15,
                            1.2
                        ],
                        [
                            20,
                            4
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "interactive": true
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_motorway_link",
            "paint": {
                "line-color": "#fc8",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "road_motorway_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_service_track",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15.5,
                            0
                        ],
                        [
                            16,
                            2
                        ],
                        [
                            20,
                            7.5
                        ]
                    ]
                }
            },
            "ref": "road_service_track_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_link",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "road_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_street",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            13.5,
                            0
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                },
                "line-opacity": 1
            },
            "ref": "road_street_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_secondary_tertiary",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            8,
                            0.5
                        ],
                        [
                            20,
                            13
                        ]
                    ]
                }
            },
            "ref": "road_secondary_tertiary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_trunk_primary",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "road_trunk_primary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "id": "road_motorway",
            "paint": {
                "line-color": "#fc8",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "road_motorway_casing"
        },
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
                    "major_rail"
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
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "interactive": true
        },
        {
            "id": "road_major_rail_hatching",
            "paint": {
                "line-color": "#bbb",
                "line-dasharray": [
                    0.2,
                    8
                ],
                "line-width": {
                    "base": 1.4,
                    "stops": [
                        [
                            14.5,
                            0
                        ],
                        [
                            15,
                            3
                        ],
                        [
                            20,
                            8
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849345966.4436"
            },
            "interactive": true,
            "ref": "road_major_rail"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "==",
                    "class",
                    "motorway_link"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_motorway_link_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "in",
                    "class",
                    "service",
                    "track"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_service_track_casing",
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "==",
                    "class",
                    "link"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_link_casing",
            "paint": {
                "line-color": "#e9ac77",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12,
                            1
                        ],
                        [
                            13,
                            3
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "in",
                    "class",
                    "street",
                    "street_limited"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_street_casing",
            "paint": {
                "line-color": "#cfcdca",
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "in",
                    "class",
                    "secondary",
                    "tertiary"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_secondary_tertiary_casing",
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
                "line-opacity": 1
            },
            "source-layer": "road"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "in",
                    "class",
                    "trunk",
                    "primary"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_trunk_primary_casing",
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
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "==",
                    "class",
                    "motorway"
                ]
            ],
            "type": "line",
            "source": "mapbox",
            "id": "bridge_motorway_casing",
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
        {
            "id": "bridge_path_pedestrian",
            "type": "line",
            "source": "mapbox",
            "source-layer": "road",
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
                        "==",
                        "structure",
                        "bridge"
                    ],
                    [
                        "in",
                        "class",
                        "path",
                        "pedestrian"
                    ]
                ]
            ],
            "paint": {
                "line-color": "#cba",
                "line-dasharray": [
                    1.5,
                    0.75
                ],
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15,
                            1.2
                        ],
                        [
                            20,
                            4
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "interactive": true
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_motorway_link",
            "paint": {
                "line-color": "#fc8",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "bridge_motorway_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_service_track",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            15.5,
                            0
                        ],
                        [
                            16,
                            2
                        ],
                        [
                            20,
                            7.5
                        ]
                    ]
                }
            },
            "ref": "bridge_service_track_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_link",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            12.5,
                            0
                        ],
                        [
                            13,
                            1.5
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                }
            },
            "ref": "bridge_link_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_street",
            "paint": {
                "line-color": "#fff",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            13.5,
                            0
                        ],
                        [
                            14,
                            2.5
                        ],
                        [
                            20,
                            11.5
                        ]
                    ]
                },
                "line-opacity": 1
            },
            "ref": "bridge_street_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_secondary_tertiary",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            10
                        ]
                    ]
                }
            },
            "ref": "bridge_secondary_tertiary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_trunk_primary",
            "paint": {
                "line-color": "#fea",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "bridge_trunk_primary_casing"
        },
        {
            "interactive": true,
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "id": "bridge_motorway",
            "paint": {
                "line-color": "#fc8",
                "line-width": {
                    "base": 1.2,
                    "stops": [
                        [
                            6.5,
                            0
                        ],
                        [
                            7,
                            0.5
                        ],
                        [
                            20,
                            18
                        ]
                    ]
                }
            },
            "ref": "bridge_motorway_casing"
        },
        {
            "id": "bridge_major_rail",
            "type": "line",
            "source": "mapbox",
            "source-layer": "road",
            "filter": [
                "all",
                [
                    "==",
                    "structure",
                    "bridge"
                ],
                [
                    "==",
                    "class",
                    "major_rail"
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
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "interactive": true
        },
        {
            "id": "bridge_major_rail_hatching",
            "paint": {
                "line-color": "#bbb",
                "line-dasharray": [
                    0.2,
                    8
                ],
                "line-width": {
                    "base": 1.4,
                    "stops": [
                        [
                            14.5,
                            0
                        ],
                        [
                            15,
                            3
                        ],
                        [
                            20,
                            8
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849334699.1902"
            },
            "interactive": true,
            "ref": "bridge_major_rail"
        },
        {
            "interactive": true,
            "layout": {
                "line-join": "round"
            },
            "metadata": {
                "mapbox:group": "1444849307123.581"
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
            "metadata": {
                "mapbox:group": "1444849307123.581"
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
                "line-cap": "round"
            },
            "metadata": {
                "mapbox:group": "1444849307123.581"
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
            "metadata": {
                "mapbox:group": "1444849307123.581"
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
            "metadata": {
                "mapbox:group": "1444849307123.581"
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
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 5,
                "text-size": 12
            },
            "metadata": {
                "mapbox:group": "1444849320558.5054"
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
                "text-color": "#74aee9",
                "text-halo-width": 1.5,
                "text-halo-color": "rgba(255,255,255,0.7)"
            },
            "source-layer": "water_label"
        },
        {
            "interactive": true,
            "minzoom": 16,
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
            "metadata": {
                "mapbox:group": "1444849297111.495"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "scalerank",
                    4
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "poi_label_4",
            "paint": {
                "text-color": "#666",
                "text-halo-color": "#ffffff",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "poi_label"
        },
        {
            "interactive": true,
            "minzoom": 15,
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
            "metadata": {
                "mapbox:group": "1444849297111.495"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "scalerank",
                    3
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "poi_label_3",
            "paint": {
                "text-color": "#666",
                "text-halo-color": "#ffffff",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "poi_label"
        },
        {
            "interactive": true,
            "minzoom": 14,
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
            "metadata": {
                "mapbox:group": "1444849297111.495"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "scalerank",
                    2
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "poi_label_2",
            "paint": {
                "text-color": "#666",
                "text-halo-color": "#ffffff",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "poi_label"
        },
        {
            "layout": {
                "text-size": 12,
                "icon-image": "{maki}-11",
                "text-font": [
                    "Open Sans Semibold",
                    "Arial Unicode MS Bold"
                ],
                "text-padding": 2,
                "visibility": "visible",
                "text-offset": [
                    0,
                    0.6
                ],
                "text-anchor": "top",
                "text-field": "{name_en}",
                "text-max-width": 9
            },
            "metadata": {
                "mapbox:group": "1444849297111.495"
            },
            "type": "symbol",
            "source": "mapbox",
            "id": "rail_station_label",
            "paint": {
                "text-color": "#666",
                "text-halo-color": "#ffffff",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "rail_station_label",
            "interactive": true
        },
        {
            "interactive": true,
            "minzoom": 13,
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
            "metadata": {
                "mapbox:group": "1444849297111.495"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "scalerank",
                    1
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "poi_label_1",
            "paint": {
                "text-color": "#666",
                "text-halo-color": "#ffffff",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "poi_label"
        },
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
            "metadata": {
                "mapbox:group": "1444849297111.495"
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
                            13,
                            12
                        ],
                        [
                            14,
                            13
                        ]
                    ]
                },
                "symbol-placement": "line"
            },
            "metadata": {
                "mapbox:group": "1456163609504.0715"
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
                "text-color": "#765",
                "text-halo-width": 1,
                "text-halo-blur": 0.5
            },
            "source-layer": "road_label"
        },
        {
            "interactive": true,
            "minzoom": 4,
            "layout": {
                "text-field": "{ref}",
                "text-font": [
                    "Open Sans Semibold",
                    "Arial Unicode MS Bold"
                ],
                "text-size": 11,
                "icon-image": "motorway_{reflen}",
                "symbol-placement": {
                    "base": 1,
                    "stops": [
                        [
                            10,
                            "point"
                        ],
                        [
                            11,
                            "line"
                        ]
                    ]
                },
                "symbol-spacing": 500,
                "text-rotation-alignment": "viewport",
                "icon-rotation-alignment": "viewport"
            },
            "metadata": {
                "mapbox:group": "1456163609504.0715"
            },
            "filter": [
                "<=",
                "reflen",
                6
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "road_label_highway_shield",
            "paint": {},
            "source-layer": "road_label"
        },
        {
            "interactive": true,
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
                            12,
                            10
                        ],
                        [
                            15,
                            14
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849272561.29"
            },
            "filter": [
                "in",
                "type",
                "hamlet",
                "suburb",
                "neighbourhood",
                "island",
                "islet"
            ],
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
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Regular",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 8,
                "text-size": {
                    "base": 1.2,
                    "stops": [
                        [
                            10,
                            12
                        ],
                        [
                            15,
                            22
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849272561.29"
            },
            "filter": [
                "==",
                "type",
                "village"
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "place_label_village",
            "paint": {
                "text-color": "#333",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 1.2
            },
            "source-layer": "place_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Regular",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 8,
                "text-size": {
                    "base": 1.2,
                    "stops": [
                        [
                            10,
                            14
                        ],
                        [
                            15,
                            24
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849272561.29"
            },
            "filter": [
                "==",
                "type",
                "town"
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "place_label_town",
            "paint": {
                "text-color": "#333",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 1.2
            },
            "source-layer": "place_label"
        },
        {
            "interactive": true,
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
                            24
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849272561.29"
            },
            "filter": [
                "==",
                "type",
                "city"
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
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-letter-spacing": 0.2,
                "symbol-placement": "line",
                "text-size": {
                    "stops": [
                        [
                            3,
                            11
                        ],
                        [
                            4,
                            12
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    ">=",
                    "labelrank",
                    4
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_line_4",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 6,
                "text-letter-spacing": 0.2,
                "symbol-placement": "point",
                "text-size": {
                    "stops": [
                        [
                            3,
                            11
                        ],
                        [
                            4,
                            12
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    ">=",
                    "labelrank",
                    4
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_4",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-letter-spacing": 0.2,
                "symbol-placement": "line",
                "text-size": {
                    "stops": [
                        [
                            3,
                            11
                        ],
                        [
                            4,
                            14
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    "==",
                    "labelrank",
                    3
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_line_3",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 5,
                "text-letter-spacing": 0.2,
                "symbol-placement": "point",
                "text-size": {
                    "stops": [
                        [
                            3,
                            11
                        ],
                        [
                            4,
                            14
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "labelrank",
                    3
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_point_3",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-letter-spacing": 0.2,
                "symbol-placement": "line",
                "text-size": {
                    "stops": [
                        [
                            3,
                            14
                        ],
                        [
                            4,
                            16
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    "==",
                    "labelrank",
                    2
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_line_2",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 5,
                "text-letter-spacing": 0.2,
                "symbol-placement": "point",
                "text-size": {
                    "stops": [
                        [
                            3,
                            14
                        ],
                        [
                            4,
                            16
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "labelrank",
                    2
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_point_2",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-letter-spacing": 0.2,
                "symbol-placement": "line",
                "text-size": {
                    "stops": [
                        [
                            3,
                            18
                        ],
                        [
                            4,
                            22
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "LineString"
                ],
                [
                    "==",
                    "labelrank",
                    1
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_line_1",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Italic",
                    "Arial Unicode MS Regular"
                ],
                "text-field": "{name_en}",
                "text-max-width": 5,
                "text-letter-spacing": 0.2,
                "text-line-height": 1.6,
                "symbol-placement": "point",
                "text-offset": [
                    0,
                    2.4
                ],
                "text-size": {
                    "stops": [
                        [
                            3,
                            18
                        ],
                        [
                            4,
                            22
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849258897.3083"
            },
            "filter": [
                "all",
                [
                    "==",
                    "$type",
                    "Point"
                ],
                [
                    "==",
                    "labelrank",
                    1
                ]
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "marine_label_point_1",
            "paint": {
                "text-color": "#74aee9",
                "text-halo-color": "rgba(255,255,255,0.7)",
                "text-halo-width": 0.75,
                "text-halo-blur": 0.75
            },
            "source-layer": "marine_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Bold",
                    "Arial Unicode MS Bold"
                ],
                "text-field": "{name_en}",
                "text-max-width": 6.25,
                "text-transform": "uppercase",
                "text-size": {
                    "stops": [
                        [
                            4,
                            11
                        ],
                        [
                            6,
                            15
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849242106.713"
            },
            "filter": [
                ">=",
                "scalerank",
                4
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "country_label_4",
            "paint": {
                "text-color": "#334",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 2,
                "text-halo-blur": 1
            },
            "source-layer": "country_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Bold",
                    "Arial Unicode MS Bold"
                ],
                "text-field": "{name_en}",
                "text-max-width": 6.25,
                "text-transform": "uppercase",
                "text-size": {
                    "stops": [
                        [
                            3,
                            11
                        ],
                        [
                            7,
                            17
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849242106.713"
            },
            "filter": [
                "==",
                "scalerank",
                3
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "country_label_3",
            "paint": {
                "text-color": "#334",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 2,
                "text-halo-blur": 1
            },
            "source-layer": "country_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Bold",
                    "Arial Unicode MS Bold"
                ],
                "text-field": "{name_en}",
                "text-max-width": 6.25,
                "text-transform": "uppercase",
                "text-size": {
                    "stops": [
                        [
                            2,
                            11
                        ],
                        [
                            5,
                            17
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849242106.713"
            },
            "filter": [
                "==",
                "scalerank",
                2
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "country_label_2",
            "paint": {
                "text-color": "#334",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 2,
                "text-halo-blur": 1
            },
            "source-layer": "country_label"
        },
        {
            "interactive": true,
            "layout": {
                "text-font": [
                    "Open Sans Bold",
                    "Arial Unicode MS Bold"
                ],
                "text-field": "{name_en}",
                "text-max-width": 6.25,
                "text-transform": "uppercase",
                "text-size": {
                    "stops": [
                        [
                            1,
                            11
                        ],
                        [
                            4,
                            17
                        ]
                    ]
                }
            },
            "metadata": {
                "mapbox:group": "1444849242106.713"
            },
            "filter": [
                "==",
                "scalerank",
                1
            ],
            "type": "symbol",
            "source": "mapbox",
            "id": "country_label_1",
            "paint": {
                "text-color": "#334",
                "text-halo-color": "rgba(255,255,255,0.8)",
                "text-halo-width": 2,
                "text-halo-blur": 1
            },
            "source-layer": "country_label"
        }
    ]
}