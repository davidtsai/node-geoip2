{
  "targets": [
    {
      "target_name": "node-mmdb",
      "sources": [
        "<!@(ls -1 ./src/*.cpp)",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "./src",
        "./deps/libmaxminddb",
        "./deps/libmaxminddb/include"
      ],
      "cflags": [
        "-std=c++11",
        "-stdlib=libc++",
        "-Wno-unused-function",
        "-Wno-reorder",
        "-O3"
      ],
      "libraries": [
        '-l./deps/libmaxminddb/src/libmaxminddb.la',
      ],
      "ldflags": [
        "-stdlib=libc++",
        "-Wl, -export-dynamic"
      ],
      "cflags!":    [ '-fno-exceptions', '-fno-rtti' ],
      "cflags_cc!": [ '-fno-exceptions', '-fno-rtti' ],
      "conditions": [
          [ 'OS=="mac"', {
            "xcode_settings": {
                'OTHER_CPLUSPLUSFLAGS': [
                  '-std=c++11',
                  '-stdlib=libc++',
                  '-Wno-unused-function',
                  '-Wno-reorder',
                  '-O3'
                ],
                'GCC_ENABLE_CPP_RTTI': 'YES',
                'OTHER_LDFLAGS': ['-stdlib=libc++'],
                'MACOSX_DEPLOYMENT_TARGET': '10.7',
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
            }
        }]
      ]
    },
    {
        "target_name": "action_after_build",
        "type": "none",
        "dependencies": [ "node-mmdb" ],
        "copies": [
            {
                "files": [ "<(PRODUCT_DIR)/node-mmdb.node" ],
                "destination": "./lib"
            }
        ]
    }
  ]
}
