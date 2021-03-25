from pyecharts.charts import Bar
from pyecharts import options as opts
import os


bar = Bar()
bar.add_xaxis(["11","22","33","44"])
bar.add_yaxis("sssA",[144,22,55,33])
bar.set_global_opts(title_opts=opts.TitleOpts(title ="wwww"))
bar.render()
os.system("render.html")