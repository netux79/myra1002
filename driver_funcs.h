/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2014 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

// Convenience macros.
#ifndef _RARCH_DRIVER_FUNCS_H
#define _RARCH_DRIVER_FUNCS_H

#define audio_init_func(device, rate, latency)  driver.audio->init(device, rate, latency)
#define audio_write_func(buf, size)             driver.audio->write(driver.audio_data, buf, size)
#define audio_stop_func()                       driver.audio->stop(driver.audio_data)
#define audio_start_func()                      driver.audio->start(driver.audio_data)
#define audio_set_nonblock_state_func(state)    driver.audio->set_nonblock_state(driver.audio_data, state)
#define audio_free_func()                       driver.audio->free(driver.audio_data)
#define audio_write_avail_func()                driver.audio->write_avail(driver.audio_data)
#define audio_buffer_size_func()                driver.audio->buffer_size(driver.audio_data)

#define video_init_func(data, scale, rgb32) driver.video->init(data, scale, rgb32)
#define video_frame_func(data, width, height, pitch, msg) driver.video->frame(driver.video_data, data, width, height, pitch, msg)
#define video_set_nonblock_state_func(state) driver.video->set_nonblock_state(driver.video_data, state)
#define video_set_rotation_func(rotate) driver.video->set_rotation(driver.video_data, rotate)
#define video_viewport_info_func(info) driver.video->viewport_info(driver.video_data, info)
#define video_overlay_interface_func(iface) driver.video->overlay_interface(driver.video_data, iface)
#define video_free_func() driver.video->free(driver.video_data)
#define input_init_func() driver.input->init()
#define input_poll_func() driver.input->poll(driver.input_data)
#define input_input_state_func(retro_keybinds, port, device, index, id) driver.input->input_state(driver.input_data, retro_keybinds, port, device, index, id)
#define input_free_func() driver.input->free(driver.input_data)

static inline bool input_key_pressed_func(int key)
{
   bool ret = false;

   if (!driver.block_hotkey)
      ret = ret || driver.input->key_pressed(driver.input_data, key);

   return ret;
}

#endif /* _RARCH_DRIVER_FUNCS_H */
