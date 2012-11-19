/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "SliderAPI"

     function test_pressed() {
        compare(slider.pressed,false,"Pressed is boolean and false by default")
     }

     function test_hovered() {
        compare(slider.hovered,false,"Hovered is boolean and false by default")
     }

     function test_live() {
        compare(slider.live,false,"Live is boolean and false by default")
         var newLive = true
         slider.live = newLive
         compare(slider.live,newLive,"can set/get")
     }

     function test_maximumValue() {
         skip("https://bugs.launchpad.net/tavastia/+bug/1076764")
         compare(slider.maximumValue,1.0,"is set to 1.0 by default")
         var newMaximumValue = 20.0
         slider.maximumValue = newMaximumValue
         compare(slider.maximumValue,newMaximumValue,"can set/get")
     }

     function test_minimumValue() {
         compare(slider.minimumValue,0,"is set to 0 by default")
         var newMinimumValue = -20.0
         slider.minimumValue = newMinimumValue
         compare(slider.minimumValue,newMinimumValue,"can set/get")
     }

     function test_value() {
         compare(slider.value,0,"is set to 0 by default")
         var newValue = 5
         slider.value = newValue
         compare(slider.value,newValue,"can set/get")
     }

     Slider {
         id: slider
     }
}