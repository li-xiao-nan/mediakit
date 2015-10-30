{
  'target_defaults': {
    'conditions': [
      ['OS=="linux"', {
        'defines': [
          '__unix__',
          '_LINUX'
        ],
        'cflags': [
          '-Wall'
        ]
      }],
      ['OS=="win"', {
        'defines': [
          'WIN32'
        ],
        'msvs_configuration_attributes': {
          'CharacterSet': '1'
        },
        'msvs_settings': {
          'VCCLCompilerTool': {
            'WarningLevel': '4',
            'Detect64BitPortabilityProblems': 'true'
          }
        }
      }],
      ['OS=="mac"', {
        'defines': [
          '__unix__',
          '_MACOS'
        ],
        'cflags': [
          '-Wall'
        ]
      }]
    ],

    'configurations': {
      'Debug': {
        'defines': [
          '_DEBUG'
        ],
        'conditions': [
          ['OS=="linux" or OS=="freebsd" or OS=="openbsd"', {
            'cflags': [
              '-g3',
              '-gstabs+',
              '-O0'
            ]
          }],
          ['OS=="win"', {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'Optimization': '0',
                'MinimalRebuild': 'true',
                'BasicRuntimeChecks': '3',
                'DebugInformationFormat': '4',
                'RuntimeLibrary': '3'  # /MDd
              },
              'VCLinkerTool': {
                'GenerateDebugInformation': 'true',
                'LinkIncremental': '2'
              }
            }
          }],
          ['OS=="mac"', {
            'xcode_settings': {
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
            }
          }]
        ]
      },

      'Release': {
        'conditions': [
          ['OS=="linux" or OS=="freebsd" or OS=="openbsd"', {
            'cflags': [
              '-O3'
            ]
          }],
          ['OS=="win"', {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'Optimization': '2',
                'RuntimeLibrary': '2'  # /MD
              }
            }
          }],
          ['OS=="mac"', {
            'xcode_settings': {
              'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
              'GCC_OPTIMIZATION_LEVEL': '3',

              # -fstrict-aliasing. Mainline gcc enables
              # this at -O2 and above, but Apple gcc does
              # not unless it is specified explicitly.
              'GCC_STRICT_ALIASING': 'YES'
            }
          }]
        ]
      }
    }
  },

  'targets': [
    {
      'target_name': 'demuxer_unittest',
      'type': 'executable',  # executable, <(library)
      # 'dependencies': [
      # ],
      # 'defines': [
      # ],
      'include_dirs': [
            '../../../src',
      ],
      'sources': [
        '../base/video_decoder_config.h',
        '../base/video_decoder_config.cpp',
        '../base/audio_decoder_config.h',
        '../base/audio_decoder_config.cpp',
        '../demuxer/demuxer.h',
        '../demuxer/demuxer.cpp',
        '../demuxer/demuxer_stream.h',
        '../demuxer/demuxer_stream.cpp',
        '../demuxer/demuxer_stream_provider.h',
        '../demuxer/ffmpeg_demuxer_stream.h',
        '../demuxer/ffmpeg_demuxer_stream.cpp',
        '../demuxer/ffmpeg_demuxer.h',
        '../demuxer/ffmpeg_demuxer.cpp',
        '../../base/CommonDef.h',
        '../../base/ImageType.cpp',
        '../../base/ImageType.h',
        '../../base/SnailConfig.h',
        '../../base/SnailException.h',
        '../ffmpeg/ffmpeg_common.h',
        '../ffmpeg/ffmpeg_common.cpp',
        '../../net/file_stream_provider.h',
        '../../net/file_stream_provider.cpp',
        '../../net/io_channel.h',
        '../../net/io_channel.cpp',
        '../../net/url.h',
        '../../net/url.cpp',
        '../../net/curl/curl_adapter.h',
        '../../net/curl/curl_adapter.cpp',
        '../../base/timer/ClockTime.h',
        '../../base/timer/ClockTime.cpp',
        '../../base/timer/SnailSleep.h',
        '../../base/timer/SystemClock.h',
        '../../base/timer/SystemClock.cpp',
        '../../base/timer/VirtualClock.h',
        '../../base/timer/WallClockTimer.h',
        '../../base/timer/WallClockTimer.cpp',
        '../../base/timer/SchedulerTimer.h',
        '../../base/timer/SchedulerTimer.cpp',
        'demuxer_unittest.cpp'
        ],
      'conditions': [
         ['OS=="linux" or OS=="freebsd" or OS=="openbsd"', {
           'ldflags': [
             '-'
           ]
         }],
         ['OS=="win"', {
           'msvs_settings': {
             'VCLinkerTool': {
               'AdditionalDependencies': '',
               'conditions': [
                 ['library=="static_library"', {
                   'AdditionalLibraryDirectories': '$(OutDir)\\lib'
                 }]
               ]
             }
           }
         }],
          ['OS=="mac"', {
            'mac_framework_dirs':[
              '../../../depend/mac/framework',
              '$(SDKROOT)/System/Library/Frameworks/',
            ],
            'xcode_settings':{
            },
            'library_dirs':[
            '../../../depend/mac/lib',
            '../../../depend/mac/lib/boost',
            ],
           'libraries':
           [
            'SDL2.framework',
            'CoreFoundation.framework',
            'AVFoundation.framework',
            'AudioToolbox.framework',
            'CoreVideo.framework',
            'VideoDecodeAcceleration.framework',
            'OpenGL.framework',
            'libavcodec.a',
            'libavdevice.a',
            'libavfilter.a',
            'libavformat.a',
            'libavutil.a',
            'libz.a',
            'libiconv.a',
            'libswresample.a',
            'libswscale.a',
            'libcurl.a',
            'libglut.a',
            'libbz2.a',
            'libboost_thread.a',
            'libboost_system.a',
           ],
           'include_dirs':[
            '../../../depend/mac/include',
           ],
         }]
       ]
    }
  ]
}
