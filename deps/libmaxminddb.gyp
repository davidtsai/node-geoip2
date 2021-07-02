{
  'targets': [
    {
      'target_name': 'libmaxminddb',
      'type': 'static_library',
      'defines': [
        'HAVE_CONFIG_H'
      ],
      'include_dirs': [
        'config/<(OS)/<(target_arch)',
        'libmaxminddb/include'
      ],
      'sources': [
        './libmaxminddb/src/maxminddb.c'
      ],
      'cflags': [
        '-std=c99',
        '-O3'
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_C_LANGUAGE_STANDARD': 'c99',
            'OTHER_CFLAGS': [
              '-std=c99',
              '-O3'
            ],
            'OTHER_CPLUSPLUSFLAGS': [
              '-std=c99',
              '-O3'
            ]
          }
        }],
        ['OS=="solaris"', {
          'cflags+': [ '-std=c99' ]
        }]
      ]
    }
  ]
}
