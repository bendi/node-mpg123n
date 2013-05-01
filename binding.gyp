{
  'targets': [
    {
      'target_name': 'bindings',
      'sources': [
        'src/bindings.cc',
        'src/node_mpg123.cc'
      ],
      'dependencies': [
        'deps/mpg123/mpg123.gyp:output',
        'deps/mpg123/mpg123.gyp:mpg123'
      ],
    }
  ]
}
