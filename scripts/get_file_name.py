def GetFileNameFromPathNoExt(path):
    return path.split("\\")[-1].split("/")[-1].split(".")[0]
def GetFileNameFromPathWithExt(path):
    return path.split("\\")[-1].split("/")[-1]