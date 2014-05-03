@rem setup
@rem git clone git://github.com/marcelklehr/nodist.git
@rem create ~/.node_pre_gyprc
@rem note, for 64 builds you may need to win7 sdk terminal
@rem https://github.com/TooTallNate/node-gyp/issues/112
@rem set PATH=c:\dev2\nodist\bin;%PATH%
@rem set PATH=node_modules\.bin;%PATH%
@rem set PATH=%PATH%;c:\Python27

setlocal
SET PATH=%PATH%;node_modules\node-pre-gyp\bin

@rem 32 bit
call set NODIST_X64=0
call nodist use stable
call node -e "console.log(process.version + ' ' + process.arch)"
call node-pre-gyp clean
call npm install --build-from-source
@rem npm test
call node-pre-gyp package publish clean

@rem 64 bit
@rem cannot open input file 'kernel32.lib' http://www.microsoft.com/en-us/download/details.aspx?id=4422
call set NODIST_X64=1
call nodist use stable
call node -e "console.log(process.version + ' ' + process.arch)"
call node-pre-gyp clean
call npm install --build-from-source
@rem npm test
call node-pre-gyp package publish clean
