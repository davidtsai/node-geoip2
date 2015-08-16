{
  'targets': [
    {
      'target_name': 'configure_libmaxminddb',
      'type': 'none',
      'hard_dependency': 1,
      'conf': '<!(cd libmaxminddb; ./bootstrap; ./configure)'
    },
    {
      'target_name': 'libmaxminddb',
      'type': 'static_library',
      'dependencies': [ 'configure_libmaxminddb' ],
      'defines': [
        'HAVE_CONFIG_H'
      ],
      'include_dirs': [
        './libmaxminddb',
        './libmaxminddb/include'
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
          }
        }],
        ['OS=="solaris"', {
          'cflags+': [ '-std=c99' ]
        }]
      ]
    }
  ]
}
