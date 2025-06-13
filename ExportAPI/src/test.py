from PIL import Image
import sys

def image_to_ascii(image_path, new_width=80):
    # 打开图片并转换灰度
    img = Image.open(image_path)
    img = img.convert("L")  # 转为灰度图

    # 计算新尺寸（保持宽高比，高度减半）
    width, height = img.size
    ratio = height / width / 2  # 高度压缩一半
    new_height = int(new_width * ratio)
    img = img.resize((new_width, new_height))
    
    # 定义字符集（按视觉密度排序）
    ascii_chars = "MNHQ$OC67!:-. "
    ascii_chars = ''.join(reversed(ascii_chars))
    
    # 生成ASCII字符画
    pixels = img.getdata()
    ascii_str = ""
    for i, pixel in enumerate(pixels):
        if i % new_width == 0 and i != 0:
            ascii_str += "\n"  # 换行
        # 映射灰度值到字符
        char_index = int(pixel / 255 * (len(ascii_chars) - 1))
        ascii_str += ascii_chars[char_index]
    return ascii_str

# 使用示例
ascii_art = image_to_ascii(sys.argv[1], new_width=80)
print(ascii_art)