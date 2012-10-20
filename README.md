mosaic
======

a php extension which generates qr code

REQUIREMENTS
============

Just make sure you've already had php and gd extension installed

INSTALLATION
============

Clone the repo with submodule

    git clone --recursive https://github.com/chrisyue/mosaic.git

Compile and install mosaic

    cd mosaic
    phpize && ./configure && make && make install

enable mosaic in php.ini

    extension=mosaic.so

USAGE
=====

Description
----------- 

    resource qr_encode(string $text[, int $msize = 5[, int $margin = 10[, int $version = 3[, $int $eclevel = QR_ECLEVEL_Q]]]])

Parameters
----------

`$text` Text to be encoded

`$msize` Module size in pixel. The picture below explains what a module is.

![what is a module in qr](http://www.qrcode.com/en/images-e/prtdot4.gif)

`$margin`

`$version` A 1 to 40 integer. You can make the qr code contain more data by increasing this parameter

`$eclevel` Error correction level. there are 4 values: `QR_ECLEVEL_L`, `QR_ECLEVEL_M`, `QR_ECLEVEL_Q`, `QR_ECLEVEL_H`.
This value indicates the ability of qr code error correction.
For example, if the `QR_ECLEVEL_H`(the highest level) is set, 
if less than 30% of the area of the qr code is damaged,
it can still be recognized and all the data in this qr can be restored

For more information about version and error correction level, please visit the
[wikipedia page](http://en.wikipedia.org/wiki/QR_code#Error_correction#Storage)

Return Values
-------------

return image resource

Examples
--------

    <?php

    header('Content-type: image/png');

    $im = qr_encode('https://github.com/chrisyue/mosaic');

    imagestring($im, 5, 65, 80, 'Mosaic', imagecolorallocate($im, 128, 128, 128));

    imagepng($im);

    imagedestroy($im);

LICENSING INFORMATION
=====================

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the conditions mentioned
in the accompanying LICENSE file are met (BSD, revised).          

CONTACT
=======

Follow me or fork this repo at https://github.com/chrisyue/mosaic

or send mails to blizzchris (gmail.com)
