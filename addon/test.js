
// Load the native addon using the 'bindings' module
// This will look for the compiled .node file in various places
const bindings = require('bindings')
const addon = bindings('api')


// Try the verifyLogin function
const result = addon.verifyLogin('admin', 'admin123')

// Should print: "Hello from C++! You said: This is a test"
console.log(result)

const base64Encoded = addon.base64Encode('火花塞的更换周期取决于其材质。普通镍合金火花塞一般建议3-5万公里更换，铂金火花塞可使用6-8万公里，而铱金火花塞则可以使用8-10万公里或更久', false)
console.log('Base64 Encoded:', base64Encoded)

const base64Decoded = addon.base64Decode(base64Encoded.replace(/\n/g, ''))
console.log('Base64 Decoded:', base64Decoded)
