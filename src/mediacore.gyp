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
      'target_name': 'mediacore',
      'type': 'static_library',  # executable, <(library)
      # 'dependencies': [
      # ],
      # 'defines': [
      # ],
      'include_dirs': [
            '../src',
      ],
      'sources': [
        'media/base/video_decoder_config.h',
        'media/base/video_decoder_config.cpp',
        'media/base/audio_decoder_config.h',
        'media/base/audio_decoder_config.cpp',
        'media/decoder/decoder.h'
        'media/demuxer/demuxer.h',
        'media/demuxer/demuxer.cpp',
        'media/demuxer/demuxer_stream.h',
        'media/demuxer/demuxer_stream.cpp',
        'media/demuxer/demuxer_stream_provider.h',
        'media/demuxer/ffmpeg_demuxer_stream.h',
        'media/demuxer/ffmpeg_demuxer_stream.cpp',
        'media/demuxer/ffmpeg_demuxer.h',
        'media/demuxer/ffmpeg_demuxer.cpp',
        'media/av_pipeline.h',
        'media/av_pipeline.cpp',
        'media/AVPipeline.h',
        'media/AVPipeline.cpp',
        'media/AVPipelineDelegate.h',
        'media/AVPipelineDelegate.cpp',
        'media/AVPipelineObserver.h',
        'media/AVPipelineObserver.cpp',
        'base/CommonDef.h',
        'base/ImageType.cpp',
        'base/ImageType.h',
        'base/SnailConfig.h',
        'base/SnailException.h',
        'media/AudioDecodedFrame.h',
        'media/AudioDecoder.h',
        'media/MediaParser.h',
        'media/MediaParserBase.h',
        'media/MediaParserBase.cpp',
        'media/MediaParserDelegate.h',
        'media/MediaDecoder.h',
        'media/MediaDecoderImpl.h',
        'media/MediaDecoderImpl.cpp',
        'media/MediaDecoderDelegate.h',
        'media/PlayControl.h',
        'media/PlayControl.cpp',
        'media/VideoDecodedFrame.h',
        'media/VideoDecoder.h',
        'media/ffmpeg/AudioDecoderFFmpeg.h',
        'media/ffmpeg/AudioDecoderFFmpeg.cpp',
        'media/ffmpeg/MediaParserFFmpeg.h',
        'media/ffmpeg/MediaParserFFmpeg.cpp',
        'media/ffmpeg/VideoDecoderFFmpeg.h',
        'media/ffmpeg/VideoDecoderFFmpeg.cpp',
        'media/ffmpeg/ffmpeg_common.h',
        'media/ffmpeg/ffmpeg_common.cpp',
        'net/FileStreamProvider.h',
        'net/FileStreamProvider.cpp',
        'net/InputStreamProvider.h',
        'net/InputStreamProvider.cpp',
        'net/IOChannel.h',
        'net/IOChannel.cpp',
        'net/NetworkAdapter.h',
        'net/Url.h',
        'net/Url.cpp',
        'net/curl/CurlAdapter.cpp',
        'media/sound/SoundHandler.h',
        'media/sound/SoundHandler.cpp',
        'media/sound/InputStream.h',
        'media/sound/AuxStream.h',
        'media/sound/AuxStream.cpp',
        'media/sound/sdl/SoundHandlerSDL.h',
        'media/sound/sdl/SoundHandlerSDL.cpp',
        'third_party/glew/glew.c',
        'third_party/glew/GL/glew.h',
        'third_party/glew/GL/glxew.h',
        'third_party/glew/GL/wglew.h',
        'base/timer/ClockTime.h',
        'base/timer/ClockTime.cpp',
        'base/timer/SnailSleep.h',
        'base/timer/SystemClock.h',
        'base/timer/SystemClock.cpp',
        'base/timer/VirtualClock.h',
        'base/timer/WallClockTimer.h',
        'base/timer/WallClockTimer.cpp',
        'base/timer/SchedulerTimer.h',
        'base/timer/SchedulerTimer.cpp',
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
              '../depend/mac/framework',
              '$(SDKROOT)/System/Library/Frameworks/',
            ],
            'xcode_settings':{
            },
            'library_dirs':[
            '../depend/mac/lib',
            '../depend/mac/lib/boost',
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
            '../depend/mac/include',
           ],
         }]
       ]
    },
    {
      'target_name':'MediaKitDemo',
      'type':'executable',
      'product_name': 'MediaKiteDemo',
      'dependencies':[
        'mediacore'
      ],
      'sources':[
      'media/main.cpp',
      ],
      'include_dirs':[
      '../src',
      '/opt/X11/include',
      ],
      'conditions':[
        ['OS == "mac"',{
        'mac_framework_dirs':[
              '../depend/mac/framework',
              '$(SDKROOT)/System/Library/Frameworks/',
            ],
            'xcode_settings':{
            },
            'library_dirs':[
              '../depend/mac/lib',
              '../depend/mac/lib/boost',
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
            'SDL2.framework',
            'OpenGL.framework',
            'GLUT.framework',
            'libboost_system.a',
            'libboost_thread.a',
            'libcurl.a',
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
           ],
            'include_dirs':[
            '../depend/mac/include',
           ],
        }]
      ]
    }
  ]
}
