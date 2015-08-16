{
  "targets": [
    {
      "target_name": "node_mmdb",
      "sources": [
        "<!@(ls -1 ./src/*.cpp)",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "deps/config/<(OS)/<(target_arch)",
        "deps/libmaxminddb",
        "deps/libmaxminddb/src",
        "deps/libmaxminddb/include"
      ],
      "cflags": [
        "-std=c++11",
        "-Wno-unused-function",
        "-Wno-reorder",
        "-O3"
      ],
      "dependencies": [
        "deps/libmaxminddb.gyp:libmaxminddb"
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
        "dependencies": [ "node_mmdb" ],
        "copies": [
            {
                "files": [ "<(PRODUCT_DIR)/node_mmdb.node" ],
                "destination": "./lib"
            }
        ]
    }
  ]
}
