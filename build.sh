rm -rf node_modules
npm install --build-from-source
npm install aws-sdk
find . -name *.o | xargs rm
find . -name *.o.d | xargs rm
./node_modules/node-pre-gyp/bin/node-pre-gyp package publish
