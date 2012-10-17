mosaic
======

a php extension which generates qr code

INSTALL
=======

    cd /path/to/mosaic
    phpize
    ./configure
    make && make install

USAGE
=====

Description
----------- 

    bool qr_encode(string $text[, int $msize = 5[, int $margin = 10[, int $version = 3[, $int $eclevel = QR_ECLEVEL_Q]]]])

Parameters
----------

`$text` Text to be encoded

`$msize` Module size in pixel. Module means the smallest square which the qr code is composed of. 
If the module size is 1, the module is a pixel

`$margin`

`$version` A 1 to 40 integer. You can make the qr code contain more data by increasing this parameter

`$eclevel` Error correction level. there are 4 values: `QR_ECLEVEL_L`, `QR_ECLEVEL_M`, `QR_ECLEVEL_Q`, `QR_ECLEVEL_H`.
This value indicates the ability of qr code error correction.
For example, if the `QR_ECLEVEL_H`(the highest level) is set, 
if less than 30% of the area of the qr code is damaged,
it can still be recognized and all the data in this qr can be restored

For more information about version and error correction level, please visit the
[wikipedia page](http://en.wikipedia.org/wiki/QR_code#Error_correction#Storage)

LICENSING INFORMATION
=====================

CONTACT
=======

Follow me or fork this repo at https://github.com/chrisyue/mosaic

or send me mails to blizzchris (gmail.com)
