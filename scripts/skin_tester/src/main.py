#######################

import dearpygui.dearpygui as dpg

import ctypes
import ctypes.wintypes
import os
import sys
import psutil
import requests
from termcolor import colored
import subprocess
import shutil

from PIL import Image
import sys

def RGBToVramBGR(colour):
    # Extract the RGB components
    r = colour[0] >> 3  # Bottom 3 bits are lost
    g = colour[1] >> 3  # Bottom 3 bits are lost
    b = colour[2] >> 3  # Bottom 3 bits are lost
    
    # Combine them into a single 16-bit value
    return (b << 10) | (g << 5) | r


def RGBToVramBGRTransparent(colour):
    # Extract the RGB components
    r = colour[0] >> 3  # Bottom 3 bits are lost
    g = colour[1] >> 3  # Bottom 3 bits are lost
    b = colour[2] >> 3  # Bottom 3 bits are lost
    
    # Combine into a 15-bit value
    converted = (b << 10) | (g << 5) | r
    
    converted |= 0x8000  # Set the most significant bit for transparency

    return converted


def ConvertTo15BitBinFile(path):
    filename = path
    img = Image.open(filename)
        
    width, height = img.size
    print('width :', width)
    print('height:', height)

    rgb_img = img.convert('RGB')

    bmp_data = []
    sixteen_bit_bmp_data = [int]

    for y in range(height):
        for x in range(width):
            pixel = rgb_img.getpixel((x, y))
            #print(pixel)
            r, g, b = pixel
            print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
            
            # bmp_data.append(r)
            # bmp_data.append(g)
            # bmp_data.append(b)
            
            sixteen_bit_bmp_data.append(RGBToVramBGR(pixel))
            
    with open(filename + "_temp", "wb+") as file:
            for i, data in enumerate(sixteen_bit_bmp_data):
                
                # Needs to be alligned to an int, so place padding after the first section
                if i == 46:
                    file.write(int(0).to_bytes(2, signed=False, byteorder="little"))
                    
                file.write(data.to_bytes(2, signed=False, byteorder='little'))

    with open(filename + "_temp", 'rb') as in_file:
        with open(filename.split(".")[0] + ".bin", 'wb') as out_file:
            out_file.write(in_file.read()[1:])
            
def ConvertTo15BitBinFileTransparent(path):
    filename = path
    img = Image.open(filename)
        
    width, height = img.size
    print('width :', width)
    print('height:', height)

    rgb_img = img.convert('RGB')

    bmp_data = []
    sixteen_bit_bmp_data = [int]

    for y in range(height):
        for x in range(width):
            pixel = rgb_img.getpixel((x, y))
            #print(pixel)
            r, g, b = pixel
            print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
            
            # bmp_data.append(r)
            # bmp_data.append(g)
            # bmp_data.append(b)
            
            sixteen_bit_bmp_data.append(RGBToVramBGRTransparent(pixel))
            
    with open(filename + "_temp", "wb+") as file:
            for i, data in enumerate(sixteen_bit_bmp_data):
                
                # Needs to be alligned to an int, so place padding after the first section
                if i == 46:
                    file.write(int(0).to_bytes(2, signed=False, byteorder="little"))
                    
                file.write(data.to_bytes(2, signed=False, byteorder='little'))

    with open(filename + "_temp", 'rb') as in_file:
        with open(filename.split(".")[0] + ".bin", 'wb') as out_file:
            out_file.write(in_file.read()[1:])
            
####################

def ConvertFlameColorData(tip_color, base_color, left_color, right_color):
    flame_bytes = b""
    
    flame_bytes += int(tip_color[0]).to_bytes(1, 'big')
    flame_bytes += int(tip_color[1]).to_bytes(1, 'big')
    flame_bytes += int(tip_color[2]).to_bytes(1, 'big')
    flame_bytes += int(0x30).to_bytes(1, 'big')
    
    flame_bytes += int(base_color[0]).to_bytes(1, 'big')
    flame_bytes += int(base_color[1]).to_bytes(1, 'big')
    flame_bytes += int(base_color[2]).to_bytes(1, 'big')
    flame_bytes += int(0x30).to_bytes(1, 'big')
    
    flame_bytes += int(left_color[0]).to_bytes(1, 'big')
    flame_bytes += int(left_color[1]).to_bytes(1, 'big')
    flame_bytes += int(left_color[2]).to_bytes(1, 'big')
    flame_bytes += int(0x30).to_bytes(1, 'big')
    
    flame_bytes += int(right_color[0]).to_bytes(1, 'big')
    flame_bytes += int(right_color[1]).to_bytes(1, 'big')
    flame_bytes += int(right_color[2]).to_bytes(1, 'big')
    flame_bytes += int(0x30).to_bytes(1, 'big')
    
    print("Flame Bytes: ")
    print(' '.join(f'{byte:02x}' for byte in flame_bytes))
    return flame_bytes
    
######################


# Define constants
PROCESS_ALL_ACCESS = 0x1F0FFF
PAGE_EXECUTE_READWRITE = 0x40
PROCESS_VM_OPERATION = 0x8
PROCESS_VM_READ = 0x10
PROCESS_VM_WRITE = 0x20

# Define some data types
DWORD = ctypes.c_uint
HANDLE = ctypes.wintypes.HANDLE
HMODULE = ctypes.wintypes.HMODULE
LPSTR = ctypes.c_char_p
MAX_PATH = 260  # Maximum path length

kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
psapi = ctypes.WinDLL('psapi', use_last_error=True)

url = "http://127.0.0.1:8080"
api_url = url + "/api/v1/cpu/ram/raw"


# Function to get process ID by a string. It doesn't have to be full, it can just be the start
def get_pid(process_name):
    process_name_lower = process_name.lower()
    for proc in psutil.process_iter(attrs=['pid', 'name']):
        #print(proc.info['name'])
        if proc.info['name'].lower().startswith(process_name_lower):
            return proc.info['pid']
    print(colored("Could not find PID for proccess starting with " + process_name, "red"))
    return None

def GetBaseAddressFromEXE(process_handle, process_name):
    base_address = None
    
    # Define an array to hold module handles
    module_handles = (HMODULE * 1024)()
    cb_needed = DWORD()

    if psapi.EnumProcessModules(process_handle, module_handles, ctypes.sizeof(module_handles), ctypes.byref(cb_needed)):
        for i in range(cb_needed.value // ctypes.sizeof(HMODULE)):
            module_name = (ctypes.c_char * MAX_PATH)()
            if psapi.GetModuleBaseNameA(process_handle, ctypes.c_ulonglong(module_handles[i]), module_name, MAX_PATH) > 0:
                module_name_str = module_name.value.decode('utf-8')
                if module_name_str.lower() == process_name.lower():
                    base_address = module_handles[i]
                    print(colored(f"Base address of {process_name}: {base_address:X}", "blue"))
                    break
    
    return base_address

def read_process_memory(handle, address, size):
    buffer = (ctypes.c_char * size)()
    bytes_read = ctypes.c_size_t()
    
    result = ctypes.windll.kernel32.ReadProcessMemory(handle, address, buffer, size, ctypes.byref(bytes_read))
    
    if result == 0:
        error_code = ctypes.windll.kernel32.GetLastError()
        print(colored(f"ReadProcessMemory failed with error code: {error_code}", "red"))
        return None
    
    return buffer.raw[:bytes_read.value]

# Function to write data to process memory
def write_to_process_memory(handle, address, data, size):
    buffer = (ctypes.c_char * size).from_buffer_copy(data)
    bytes_written = ctypes.c_size_t()
    result = ctypes.windll.kernel32.WriteProcessMemory(handle, address, buffer, size, ctypes.byref(bytes_written))
    return result != 0

# Function to change memory protection
def change_memory_protection(handle, address, size, protection):
    old_protection = ctypes.c_ulong()
    ctypes.windll.kernel32.VirtualProtectEx(handle, address, size, protection, ctypes.byref(old_protection))
    return old_protection

def InjectIntoEmu(tip_color, base_color, left_color, right_color, fire_bmp, spyro_bmp):
    
    emu_info = {
    'name': 'duckstation',
    'base_exe_dll_name': 'duckstation-qt-x64-ReleaseLTCG.exe',
    'main_ram_offset': 0x87E6B0,
    'ptr': True,
    'double_ptr': False,
    'base': True,
    }

    # Get the PID of the target process
    emu_pid = get_pid(emu_info['name'])
    if emu_pid is None:
        exit_message = colored(f"Could not find {emu_info['name']} Process!", "red")
        print(exit_message)
        return "Could not find {emu_info['name']} Process!"
        
    emu_handle = ctypes.windll.kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, emu_pid)
    
    if emu_info['base']:
        base_address = GetBaseAddressFromEXE(emu_handle, f"{emu_info['base_exe_dll_name']}")

        
        #Get pointer + base address like duckstation and pcsx2 1.7 and dolphin
    if emu_info['ptr']:
        try:
            ptr_address = read_process_memory(emu_handle, ctypes.c_ulonglong(base_address + emu_info['main_ram_offset']), 8)
            pointer_value = int.from_bytes(ptr_address, byteorder='little', signed=False)  
        except:
            exit_message = colored(f"Failed to read pointer to main ram", "red")
            print(exit_message)
            return "Failed to read pointer to main ram"
        
        main_ram = pointer_value    
        print(colored(f"Address of main ram in emu: {hex(main_ram)}", "blue"))
        
    #Get base address + pointer, to a pointer like dolphin
    if emu_info['double_ptr']:
        try:
            pointer_pointer_address = read_process_memory(emu_handle, ctypes.c_ulonglong(base_address + emu_info['main_ram_offset']), 8)
            pointer_pointer_value = int.from_bytes(pointer_pointer_address, byteorder='little', signed=False)  
            ptr_address = read_process_memory(emu_handle, ctypes.c_ulonglong(pointer_pointer_value), 8)
            pointer_value = int.from_bytes(ptr_address, byteorder='little', signed=False)  
        except:
            exit_message = colored(f"Failed to read pointer to main ram", "red")
            print(exit_message)
            return "Failed to read pointer to main ram"
        
        main_ram = pointer_value    
        print(colored(f"Address of main ram in emu: {hex(main_ram)}", "blue"))
        
    #else just add the offset to the base address, like for bizhawk
    elif not emu_info['ptr'] and emu_info['base']:
        main_ram = base_address + emu_info['main_ram_offset']
    #situation for if just a pointer, haven't encountered yet
    elif not emu_info['base'] and emu_info['ptr']:
        pass
    #else just a global address, like mednafen/pcsx2 1.6.0
    elif not emu_info['base']:
        main_ram = emu_info['address']
        
    #Injecting
    
    address_to_write_spyro_bmp_to = ctypes.c_ulonglong(main_ram + int("0x0740B0", base=16))
    address_to_write_fire_bmp_to = ctypes.c_ulonglong(main_ram + int("0x0742B0", base=16))
    address_to_write_triangle_to = ctypes.c_ulonglong(main_ram + int("0x06E1A8", base=16))
    address_to_write_bool_to = ctypes.c_ulonglong(main_ram + int("0x074300", base=16))
    
    spyro_bmp_path = spyro_bmp
    spyro_bin_path = None
    spyro_bmp_data = b""
    
    fire_bmp_path = fire_bmp
    fire_bmp_data = b""
    fire_bin_path = None
    
    triangle_data = ConvertFlameColorData(base_color, tip_color, left_color, right_color)
    
    if spyro_bmp_path:
        spyro_bin_path = spyro_bmp.split(".")[0] + ".bin"
        ConvertTo15BitBinFile(spyro_bmp)
    if fire_bmp_path:
        fire_bin_path = fire_bmp.split(".")[0] + ".bin"
        ConvertTo15BitBinFileTransparent(fire_bmp)
    
    if spyro_bin_path:
        with open(spyro_bin_path, "rb+") as bmp_file:
            spyro_bmp_data = bmp_file.read()
            
    if fire_bin_path:
        with open(fire_bin_path, "rb+") as bmp_file:
            fire_bmp_data = bmp_file.read()

    
    #print(hex(main_ram + int("0x80070000", base=16)))
    old_protection = change_memory_protection(emu_handle, address_to_write_spyro_bmp_to, 0x300, PAGE_EXECUTE_READWRITE)
    old_protection = change_memory_protection(emu_handle, address_to_write_fire_bmp_to, 0x300, PAGE_EXECUTE_READWRITE)
    old_protection = change_memory_protection(emu_handle, address_to_write_triangle_to, 0x300, PAGE_EXECUTE_READWRITE)
    
    if spyro_bmp_path:
        if write_to_process_memory(emu_handle, address_to_write_spyro_bmp_to, spyro_bmp_data, len(spyro_bmp_data)):
            print(colored(f"Successfully placed spyro bmp at: 0x0740B0", "yellow"))
      
    if fire_bmp_path:  
        if write_to_process_memory(emu_handle, address_to_write_fire_bmp_to, fire_bmp_data, len(fire_bmp_data)):
            print(colored(f"Successfully placed fire bmp at: 0x0742B0", "yellow"))
        
    if triangle_data:
        if write_to_process_memory(emu_handle, address_to_write_triangle_to, triangle_data, len(triangle_data)):
            print(colored(f"Successfully placed triangles at: 0x06E1A8", "yellow"))
            
    if write_to_process_memory(emu_handle, address_to_write_bool_to, b"0x00000001", 4):
        print(colored(f"Successfully Injected bool!: 0x074300", "yellow"))
    else:
        #change_memory_protection(emu_handle, address_to_write_to, len(mod_data[cave[0]]), old_protection)
        error_code = ctypes.windll.kernel32.GetLastError()
        exit_message = colored(f"Failed to write to memory. Code: {error_code}", "red")
        print(exit_message)
        return exit_message
        
        

    # function_address = kernel32.GetProcAddress(emu_handle, b"eeMem")
    # error_code = ctypes.windll.kernel32.GetLastError()
    # if not function_address:
    #     exit_message = colored(f"Failed. Code: {error_code}", "red")
    #     print(exit_message)
    #     raise Exception("Failed to get the function address")
    
    # print("TEST", function_address)

    # Close the process handle
    ctypes.windll.kernel32.CloseHandle(emu_handle)
    return f"Successfully injected mod into {emu_info['name']}!"

###################################

def button_callback(sender, app_data):
    # Retrieve the color values from each color picker
    dpg.show_item("notification_text")
    dpg.set_value("notification_text", "Injecting...")
    
    color1 = dpg.get_value("Base Color")
    color2 = dpg.get_value("Tip Color")
    color3 = dpg.get_value("Right Color")
    color4 = dpg.get_value("Left Color")
    
    # Retrieve the file paths
    file_path1 = dpg.get_value("File Path 1")
    file_path2 = dpg.get_value("File Path 2")
    
    print(f"Base Color: {color1}")
    print(f"Tip Color: {color2}")
    print(f"Right Color: {color3}")
    print(f"Left Color: {color4}")
    print(f"File Path 1: {file_path1}")
    print(f"File Path 2: {file_path2}")
    
    # Call your custom function with the selected colors and file paths
    InjectIntoEmu(color1, color2, color3, color4, file_path1, file_path2)
    
    # Show notification
    dpg.set_value("notification_text", "Injection Complete!")
    dpg.show_item("notification_text")


def file_dialog_callback(sender, app_data):
    # Update the input text with the selected file path
    dpg.set_value(sender, app_data['file_path_name'])

if __name__ == "__main__":

    dpg.create_context()

    # Create the main application window
    with dpg.window(label="Color Picker Window", width=800, height=800):
        dpg.add_text("Edit Flame Triangle Colors:")
        
        # Create first row of color wheels
        with dpg.group(horizontal=True):
            dpg.add_color_picker((0xF0, 0xF0, 60, 255), width=200, label="Base Color", tag="Base Color")
            dpg.add_color_picker((0xD0, 0x90, 0x40, 255), width=200, label="Tip Color", tag="Tip Color")

        # Create second row of color wheels
        with dpg.group(horizontal=True):
            dpg.add_color_picker((0x90, 0x60, 0x20, 255), width=200, label="Right Color", tag="Right Color")
            dpg.add_color_picker((0xA0, 0x30, 0x10, 255), width=200, label="Left Color", tag="Left Color")
        

        dpg.add_spacing(count=5)
        
        # Add file path inputs and file dialogs
        dpg.add_text("Select File Paths:")
        
        # Add spacing between the title and the file path inputs
        dpg.add_spacing(count=2)

        # Group for the first file path input and button
        with dpg.group(horizontal=True):
            dpg.add_input_text(label="Flame Skin .bmp", width=300, tag="File Path 1", readonly=True)
            dpg.add_button(label="Browse", callback=lambda: dpg.show_item("file_dialog_1"))

        # Add spacing between the file path groups
        dpg.add_spacing(count=2)
        
        # Group for the second file path input and button
        with dpg.group(horizontal=True):
            dpg.add_input_text(label="Spyro Skin .bmp", width=300, tag="File Path 2", readonly=True)
            dpg.add_button(label="Browse", callback=lambda: dpg.show_item("file_dialog_2"))
            
        dpg.add_spacing(count=5)
        
        # Add a larger button to submit and print selected colors
        dpg.add_button(label="Inject Into Duckstation", callback=button_callback, width=300, height=50)

        # Add notification text (initially hidden)
        dpg.add_text("Injection Complete!", tag="notification_text")
        dpg.hide_item("notification_text")
        
        
    # Create file dialogs
    with dpg.file_dialog(directory_selector=False, show=False, callback=lambda s, d: file_dialog_callback("File Path 1", d), tag="file_dialog_1", min_size=(600, 400), max_size=(1200, 800)):
        dpg.add_file_extension(".bmp", color=(255, 255, 255, 255))  # Show all file types

    with dpg.file_dialog(directory_selector=False, show=False, callback=lambda s, d: file_dialog_callback("File Path 2", d), tag="file_dialog_2", min_size=(600, 400), max_size=(1200, 800)):
        dpg.add_file_extension(".bmp", color=(255, 255, 255, 255))  # Show all file types

    dpg.create_viewport(title='Flame Tester', width=800, height=800)
    dpg.setup_dearpygui()
    dpg.show_viewport()

    # Every-frame loop
    while dpg.is_dearpygui_running():
        # Render each frame
        dpg.render_dearpygui_frame()

    dpg.destroy_context()